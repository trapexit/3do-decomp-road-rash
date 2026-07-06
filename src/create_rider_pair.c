#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "intrusive_list.h"
#include "racer_creation_runtime.h"
#include "rider_animation_runtime.h"

#include "audio.h"
#include "stdlib.h"

enum RiderPairCreationValue
{
  RIDER_BITS_PER_BYTE = 8,
  RIDER_PROGRESSION_SELECTION_COUNT = 3,
  RIDER_RANDOM_DELAY_MASK = 15,
  RIDER_STYLE_SHIFT = 16,
  RIDER_STYLE_MASK = 255,
  RIDER_ICON_WIDTH = 200,
  RIDER_ICON_HEIGHT = 234,
  RIDER_SPEED_PERCENT_DIVISOR = 100,
  RIDER_ANIMATION_TARGET_OFFSET = 12,
  OPPONENT_LANE_CHANGE_REDUCTION = 25,
  OPPONENT_ATTACK_REDUCTION = 15,
  OPPONENT_RECOVERY_REDUCTION = 8,
  OPPONENT_PURSUIT_REDUCTION = 4
};

static
int
divide_by_two_towards_zero(int value)
{
  return (value + (int)((unsigned int)value >>
                        (sizeof(value) * RIDER_BITS_PER_BYTE - 1))) >> 1;
}


static
unsigned char
rider_style(const RiderRuntimeSlot *runtime)
{
  return (unsigned char)(runtime->rider_flags_and_style >>
                         RIDER_STYLE_SHIFT);
}


static
void
set_rider_style(RiderRuntimeSlot *runtime,
                unsigned char     style)
{
  runtime->rider_flags_and_style =
    (runtime->rider_flags_and_style &
     ~((unsigned int)RIDER_STYLE_MASK << RIDER_STYLE_SHIFT)) |
    ((unsigned int)style << RIDER_STYLE_SHIFT);
}


static
void
reduce_opponent_timing(AiRacerObject *racer)
{
  AiOpponentTimingView *timing;

  timing = (AiOpponentTimingView *)racer;
  timing->lane_change_interval -= OPPONENT_LANE_CHANGE_REDUCTION;
  timing->attack_interval -= OPPONENT_ATTACK_REDUCTION;
  timing->recovery_interval -= OPPONENT_RECOVERY_REDUCTION;
  timing->pursuit_interval -= OPPONENT_PURSUIT_REDUCTION;
}


static
int
acquire_rider_entity_pair(const RacerCreationTemplate *object_template,
                          const RacerCreationTemplate *state_template,
                          RoadSegmentLaneRuntime      *lane,
                          int                          spawn_position,
                          AiRacerObject              **racer_out,
                          RacerEntity                **state_out)
{
  AiRacerObject *racer;
  RacerEntity *state;

  *racer_out = 0;
  *state_out = 0;
  racer = (AiRacerObject *)create_racer_entity_from_template(
    race_rider_state.object_pool, object_template,
    spawn_position, 0, lane);
  state = create_racer_entity_from_template(
    race_rider_state.state_pool, state_template,
    spawn_position, 0, lane);
  if(racer == 0 || state == 0)
    {
      if(racer != 0)
        {
          return_pooled_node_to_owner(&((RacerEntity *)racer)->list_node);
        }
      if(state != 0)
        {
          return_pooled_node_to_owner(&state->list_node);
        }
      return 0;
    }
  *racer_out = racer;
  *state_out = state;
  return 1;
}


AiRacerObject *
create_rider_pair(const RacerCreationTemplate *object_template,
                  const RacerCreationTemplate *state_template,
                  RoadSegmentLaneRuntime      *lane,
                  int                          position_offset,
                  RacerRole                    role,
                  int                          rider_slot)
{
  AiRacerObject *racer;
  AiRacerObject *player;
  RacerEntity *racer_base;
  RacerEntity *state;
  RacerEntity *player_state;
  RiderRuntimeSlot *runtime;
  RiderRuntimeSlot *player_runtime;
  RiderProfile *profile;
  SignedDivisionResult division;
  void *animation_target;
  int slot;
  int delay;
  int selector;
  int spawn_position;

  slot = (signed char)rider_slot;
  if(object_template == 0 || state_template == 0 || lane == 0 ||
     race_rider_state.object_pool == 0 || race_rider_state.state_pool == 0 ||
     slot < 0 || slot >= RACE_RIDER_COUNT ||
     race_mode_state.active_player_index < 0 ||
     race_mode_state.active_player_index >= PLAYER_PROFILE_COUNT)
    {
      return 0;
    }
  profile = &player_profiles[race_mode_state.active_player_index];
  if(profile->level < 0 ||
     profile->level >= RIDER_PROGRESSION_LEVEL_COUNT ||
     profile->rider_style < 0 ||
     profile->rider_style >= RIDER_BIKE_STYLE_COUNT ||
     (role == RACER_ROLE_OPPONENT &&
      race_rider_state.player_object == 0))
    {
      return 0;
    }
  if(role == RACER_ROLE_OPPONENT)
    {
      selector = rider_runtime_selectors[slot];
      if(selector < 0 || selector >= RACE_RIDER_COUNT)
        {
          return 0;
        }
    }

  delay = (signed char)(GetAudioTime() & RIDER_RANDOM_DELAY_MASK);
  while(((unsigned int)delay & RIDER_STYLE_MASK) != 0)
    {
      rand();
      delay = (signed char)(delay - 1);
    }

  spawn_position = lane->center_position + position_offset;
  if(!acquire_rider_entity_pair(
       object_template, state_template, lane, spawn_position,
       &racer, &state))
    {
      return 0;
    }

  racer_base = (RacerEntity *)racer;
  racer->paired_state = (AiRacerObject *)state;
  racer_base->collision_peer = state;
  racer->rider_mode = 0;
  racer->cursor_initialized = 1;
  racer->cursor_node = (AiTrackCursorNode *)racer->track_segment->clip_node;
  racer->cursor_position = racer->track_segment->clip_position;
  racer->elapsed_time = 0;
  racer->accumulated_track_delta = 0;
  racer->target_speed = 0;

  state->owner = racer_base;
  state->collision_peer = racer_base;

  division = signed_divide_with_remainder(
    RIDER_PROGRESSION_SELECTION_COUNT, rand());
  initialize_opponent_progression_record(
    (OpponentProgressionRecord **)&state->interaction_data,
    division.remainder, position_offset, slot);

  runtime = &rider_runtime_slots[slot];
  state->runtime = runtime;
  runtime->resource_item = rider_resource_items[slot];
  if(runtime->resource_item != 0)
    {
      map_cel_at_native_scale(
        (CCB *)runtime->resource_item,
        RIDER_ICON_HEIGHT, RIDER_ICON_WIDTH);
    }
  runtime->race_score = 0;
  state->pending_contact_animation_frame = 0;
  state->last_contact_track_cell = 0;

  if(role == RACER_ROLE_PLAYER && race_mode_state.game_mode == 0)
    {
      set_rider_style(runtime, profile->rider_style);
    }
  else
    {
      selector = profile->level;
      set_rider_style(runtime, rider_style_defaults[slot][selector]);
    }

  initialize_ai_racer_state(racer);
  if(rider_style(runtime) >= BIKE_SPEC_CACHE_COUNT)
    {
      return_pooled_node_to_owner(&racer_base->list_node);
      return_pooled_node_to_owner(&state->list_node);
      return 0;
    }
  apply_rider_bike_specification(
    racer, divide_by_two_towards_zero(runtime->rider_rating),
    rider_style(runtime));
  if(race_rider_state.bike_spec_cache[rider_style(runtime)] == 0)
    {
      return_pooled_node_to_owner(&racer_base->list_node);
      return_pooled_node_to_owner(&state->list_node);
      return 0;
    }

  if(role == RACER_ROLE_OPPONENT || role == RACER_ROLE_CHALLENGE)
    {
      selector = profile->level;
      division = signed_divide_with_remainder(
        RIDER_SPEED_PERCENT_DIVISOR,
        racer_base->acceleration_limit *
        opponent_speed_percent[selector]);
      racer_base->acceleration_limit = division.quotient;
      reduce_opponent_timing(racer);
    }

  initialize_rider_animation(&racer_base->animation, role);
  initialize_rider_animation(&state->animation, role);
  advance_car_animation(&racer_base->animation);

  if(role != RACER_ROLE_CHALLENGE)
    {
      if(role == RACER_ROLE_PLAYER)
        {
          animation_target =
            (char *)rider_animation_state_targets[rider_style(runtime)] +
            RIDER_ANIMATION_TARGET_OFFSET;
        }
      else
        {
          selector = rider_runtime_selectors[slot];
          player = (AiRacerObject *)race_rider_state.player_object;
          player_state = (RacerEntity *)player->paired_state;
          player_runtime = player_state->runtime;
          if(selector == rider_style(player_runtime))
            {
              animation_target =
                (char *)rider_animation_state_targets[0] +
                RIDER_ANIMATION_TARGET_OFFSET;
            }
          else
            {
              animation_target =
                (char *)rider_animation_state_targets[selector] +
                RIDER_ANIMATION_TARGET_OFFSET;
            }
        }
      state->animation_state_target = animation_target;
      racer_base->animation_state_target = animation_target;
    }

  return racer;
}

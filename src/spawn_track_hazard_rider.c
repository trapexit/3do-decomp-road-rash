#include "stdlib.h"

#include "division_helpers.h"
#include "rider_visual_resource_binding.h"
#include "rider_visual_runtime.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

void *
spawn_track_hazard_rider(int                     travel_mode,
                         int                     family_resource_inventory,
                         int                     lateral_position,
                         int                     track_offset,
                         RoadSegmentLaneRuntime *track_lane)
{
  RacerEntity *player;
  RiderVisualObject *rider;
  RoadSegmentLaneRuntime *challenge_lane;
  SignedDivisionResult division;
  int player_track_position;

  player = race_rider_state.player_object;
  if(player == 0)
    {
      player_track_position = 0;
    }
  else
    {
      player_track_position = player->paired_state->position_z;
    }

  if(travel_mode >= RIDER_TRAVEL_MODE_COUNT)
    {
      if(player == 0)
        {
          return (void *)track_offset;
        }

      division = signed_divide_with_remainder(
        RIDER_CHALLENGE_LANE_RANDOM_DIVISOR,
        (unsigned int)rand());
      challenge_lane = move_road_lane_by_offset(
        player->track_segment,
        RIDER_CHALLENGE_LANE_BASE_OFFSET +
        ((int)division.remainder <<
          RIDER_CHALLENGE_LANE_RANDOM_SHIFT));
      return (void *)spawn_challenge_rider_at_lane(challenge_lane);
    }

  rider = (RiderVisualObject *)create_racer_entity_from_template(
    rider_visual_pool, &rider_visual_template, lateral_position,
    track_offset, track_lane);
  if(rider == 0)
    {
      return 0;
    }
  reset_rider_visual_resource_binding(rider);

  rider->target_position = rider->position;
  rider->target_track_segment = rider->track_segment;
  rider->travel_mode = travel_mode;
  rider->family_resource_inventory = family_resource_inventory;
  if(lateral_position <= 0)
    {
      rider->lane_direction_index = TRACK_OBJECT_MODE_CROSS_POSITIVE;
    }
  else
    {
      rider->lane_direction_index = TRACK_OBJECT_MODE_CROSS_NEGATIVE;
    }

  rider->movement_direction = 0;
  rider->movement_force = 0;
  rider->lane_probe_pending = 1;
  rider->last_sound_tick = 0;

  division = signed_divide_with_remainder(
    RIDER_EFFECT_COOLDOWN_RANDOM_DIVISOR, (unsigned int)rand());
  rider->action_cooldown_tick =
    player_track_position + (int)division.remainder;
  rider->behavior_state = RIDER_BEHAVIOR_WAITING;
  rider->animation_ready = 0;

  rider->animation_states[RIDER_ANIMATION_MOTION] = 0;
  rider->animation_states[RIDER_ANIMATION_STANDING] = 0;
  rider->animation_states[RIDER_ANIMATION_SHAKE] = 0;
  rider->animation_states[RIDER_ANIMATION_FALL] = 0;
  rider->animation_states[RIDER_ANIMATION_ATTACK] = 0;
  rider->animation_states[RIDER_ANIMATION_FAST] = 0;
  rider->animation_states[RIDER_ANIMATION_FLAG] = 0;

  rider->animation.cans_resource = 0;
  rider->animation.animation_resource = 0;
  rider->animation.state_table = rider->animation_states;
  rider->animation.current_frame = 0;
  rider->animation.primary_state =
    (unsigned char)RIDER_ANIMATION_NONE;
  rider->animation.primary_frame = 0;
  rider->animation.secondary_state =
    (unsigned char)RIDER_ANIMATION_NONE;
  rider->animation.next_frame_tick = 0;
  rider->animation.cycle_count = 0;

  return &rider->animation;
}

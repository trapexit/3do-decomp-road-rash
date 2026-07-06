#include "rider_animation_runtime.h"
#include "rider_challenge_offsets.h"
#include "rider_contact_runtime.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_queue.h"
#include "stdlib.h"

/* Racer state, animation, and pacing updates. */



typedef struct SignedDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} SignedDivisionResult;

#define CURRENT_FRAME_TICK (*(unsigned int *)(int)&(frame_tick))

__value_in_regs SignedDivisionResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
RacerEntity *
find_nearby_active_challenge_racer(RacerEntity *player);
RacerEntity *
find_nearby_pacing_rider_in_direction(RacerEntity *rider,
                                      int          direction,
                                      int          maximum_distance);
int
schedule_race_outcome_event(int event,
                            int value);
int
absolute_int_difference(int first,
                        int second);
int
update_rider_animation_and_challenge_state(RacerEntity *rider);
/* update_rider_pacing_state keeps char * rider: initialize_ai_racer_state
 * also passes an AiRacerObject here, so no single aggregate type fits. */
void
update_rider_pacing_state(char *rider);

#define RACER_UPDATE_PERIOD 60
#define RIDER_HANDLER_ENTRY_BYTES WORD_BYTES
#define RACER_METRIC_ENTRY_BYTES 12
#define RACER_METRIC_ENTRY_COUNT 10
#define RACER_METRIC_CURRENT_A 0
#define RACER_METRIC_CURRENT_B 1
#define RACER_METRIC_CURRENT_C 2
#define RACER_METRIC_TARGET_A 30
#define RACER_METRIC_TARGET_B 31
#define RACER_METRIC_TARGET_C 32

#define RIDER_MODE_NORMAL 0
#define RIDER_MODE_RECOVERY 1
#define RIDER_MODE_STOPPED 2

#define RIDER_STATUS_EVENT_FLAG 0x10
#define RIDER_ACTION_FLAG 0x20
#define RACE_STATUS_SHIFT 24
#define ACTIVE_RACE_STATUS_COUNT 4

#define CHALLENGE_EVENT 0x16
#define CHALLENGE_PROXIMITY_EVENT 4
#define CHALLENGE_PROXIMITY_VALUE 0xB4
#define CHALLENGE_SPAWN_FRAME_SHIFT 20
#define CHALLENGE_SELECTION_THRESHOLD (-0x2700)
#define CHALLENGE_MODE_SPEED_LIMIT 0x708
#define CHALLENGE_MAX_SPEED 300
#define CHALLENGE_TRACK_DISTANCE 0x200
#define CHALLENGE_SPEED_DISTANCE 5000

#define RIDER_ANIMATION_IDLE 0
#define RIDER_ANIMATION_DISTANT 1
#define RIDER_ANIMATION_CRUISE 2
#define RIDER_ANIMATION_FAST 3
#define RIDER_ANIMATION_LOW_SPEED 4
#define RIDER_ANIMATION_EVENT 5
#define RIDER_ANIMATION_TRANSITION 6
#define RIDER_ANIMATION_STOPPED 8
#define RIDER_ANIMATION_RECOVERY_A 0x1C
#define RIDER_ANIMATION_RECOVERY_B 0x1D
#define RIDER_ANIMATION_RECOVERY_C 0x1E
#define RIDER_ANIMATION_RECOVERY_D 0x1F
#define RIDER_ANIMATION_RECOVERY_E 0x20
#define RIDER_ANIMATION_RECOVERY_TRIGGER 0x21
#define RIDER_ANIMATION_RECOVERY_TRANSITION 0x22
#define RIDER_ANIMATION_RECOVERY_ALTERNATE 0x23
#define RIDER_ANIMATION_RECOVERY_STOP 0x24
#define RIDER_ANIMATION_RECOVERY_END 0x25

#define RIDER_LOW_SPEED_THRESHOLD 300
#define RIDER_MEDIUM_SPEED_THRESHOLD 0x708
#define RIDER_FAST_SPEED_THRESHOLD 0x251C
#define RIDER_CONTROL_THRESHOLD 30
#define RIDER_ACTION_VALUE_LIMIT 100
#define RIDER_STEERING_THRESHOLD (-0x1000)
#define RIDER_RECOVERY_VALUE_LIMIT 0x1000
#define RIDER_RECOVERY_FAST_SPEED 0x1F40
#define RIDER_RECOVERY_CRUISE_SPEED 7000
#define RIDER_RECOVERY_SLOW_SPEED 2500
#define RANDOM_CHOICE_MASK 3
#define RANDOM_CHOICE_MIDDLE 2

#define RIDER_NEAR_DISTANCE 0x500
#define RIDER_MEDIUM_DISTANCE 0xA00
#define SPECIAL_RIDER_RANGE 0x5000
#define NORMAL_RIDER_RANGE RIDER_MEDIUM_DISTANCE

#define PLAYER_SLOT_DEFAULT 15
#define PLAYER_SLOT_MINIMUM 4

#define CANDIDATE_CLASS_LOW 1
#define CANDIDATE_CLASS_MEDIUM 2
#define CANDIDATE_CLASS_HIGH 3
#define CANDIDATE_LOW_BASE 16000
#define CANDIDATE_LOW_PROFILE_STEP 1500
#define CANDIDATE_LOW_SLOT_STEP 1000
#define CANDIDATE_MEDIUM_BASE 32000
#define CANDIDATE_MEDIUM_PROFILE_STEP 3000
#define CANDIDATE_MEDIUM_SLOT_STEP 2000

#define PACING_SPECIAL_COUNT_LIMIT 50
#define PACING_PERCENT_BASE 100
#define PACING_PROFILE_SCALE 6
#define PACING_DELTA_SHIFT 8
#define PACING_DELTA_MIN (-9)
#define PACING_DELTA_MAX 10
#define PACING_DELTA_INDEX_BIAS 9
#define PACING_LEFT_PRESENT 2
#define PACING_SPECIAL_PROFILE 3
#define PACING_EXCLUDED_PROFILE 4

#define DIVIDE_BY_TWO_SHIFT 1
#define DIVIDE_BY_FOUR_SHIFT 2
#define DIVIDE_BY_EIGHT_SHIFT 3
#define DIVIDE_BY_SIXTEEN_SHIFT 4
#define DIVIDE_BY_THIRTY_TWO_SHIFT 5
#define DIVIDE_BY_FOUR_BIAS 3
#define DIVIDE_BY_EIGHT_BIAS 7
#define DIVIDE_BY_SIXTEEN_BIAS 15
#define DIVIDE_BY_THIRTY_TWO_BIAS 31

/* Animation base proof: rider_state + 0x70 addresses RacerEntity.animation
 * (previous_orientation ends at 0x70; see
 * RacerPreviousOrientationOffsetCheck in racer_runtime.h). */
typedef char RiderStateAnimationOffsetIs070[
    (offsetof(RacerEntity, animation) == 0x70) ? 1 : -1];

void
queue_challenge_audio_event(int        event_type,
                            const int *source_position,
                            const int *source_forward_velocity)
{
  queue_spatial_audio_event(event_type, source_position,
                            source_forward_velocity);
}


void
racer_state_update(RacerEntity *racer_state)
{
  RacerEntity *rider;
  RiderInteractionRuntime *racer_data;
  RacerEntity *challenge_rider;
  RiderRuntimeSlot *progress;
  int handler_index;
  int rider_mode;
  int progress_value;
  int progress_limit;

  rider = racer_state->owner;
  racer_data = racer_state->interaction_data;
  challenge_rider = (RacerEntity *)race_rider_state.challenge_object;

  if(challenge_rider != 0 &&
     racer_state == challenge_rider->paired_state)
    {
      if(rider->rider_mode == RIDER_MODE_NORMAL)
        {
          queue_spatial_audio_event(
            CHALLENGE_EVENT,
            &racer_state->position_z,
            &racer_state->velocity.forward);
          goto challenge_event_complete;
        }
      else
        {
          queue_challenge_audio_event(CHALLENGE_EVENT, 0, 0);
        }
    }

challenge_event_complete:

  select_rider_update_handler(rider);
  handler_index = (signed char)racer_data->handler_index;
  rider_update_handlers[handler_index](rider);
  handler_index =
    (signed char)*(volatile int *)&racer_data->handler_index;
  racer_data->handler_elapsed[handler_index] +=
    racer_state->time_step;

  racer_data->pacing_frame_accumulator += racer_state->time_step;
  if(racer_data->pacing_frame_accumulator >=
     RACER_UPDATE_PERIOD)
    {
      update_rider_pacing_state((char *)rider);
    }

  rider_mode = rider->rider_mode;
  if(rider_mode != RIDER_MODE_RECOVERY)
    {
      progress = rider->paired_state->runtime;
      progress_value = progress->current_health;
      progress_limit = progress->recovery_health_ceiling;
      if(progress_value < progress_limit)
        {
          progress_value += rider->time_step;
          progress->current_health = progress_value;
          if(progress->recovery_health_ceiling <
             progress->current_health)
            {
              progress->current_health =
                progress->recovery_health_ceiling;
            }
        }

      rider_mode = *(volatile int *)&rider->rider_mode;
      if(rider_mode != RIDER_MODE_NORMAL &&
         rider_mode != RIDER_MODE_RECOVERY)
        {
          progress = rider->paired_state->runtime;
          progress_value = progress->current_health;
          progress_limit = progress->recovery_health_ceiling;
          if(progress_value < progress_limit)
            {
              progress_value += rider->time_step;
              progress->current_health = progress_value;
              if(progress->recovery_health_ceiling <
                 progress->current_health)
                {
                  progress->current_health =
                    progress->recovery_health_ceiling;
                }
            }
        }
    }

  update_rider_animation_and_challenge_state(rider);
}


static
void
select_primary(RiderAnimationRuntime *animation,
               int                    state)
{
  select_primary_rider_animation(animation, state);
}


static
void
select_secondary(RiderAnimationRuntime *animation,
                 int                    state)
{
  select_secondary_rider_animation(animation, state);
}


int
update_rider_animation_and_challenge_state(RacerEntity *rider)
{
  RacerEntity *rider_state;
  RiderAnimationRuntime *animation;
  RacerEntity *candidate;
  RacerEntity *player;
  RacerEntity *challenge_rider;
  RoadSegmentLaneRuntime *challenge_lane;
  int animation_state;
  int speed;
  int challenge_allowed;
  int value;
  unsigned int random;

  rider_state = rider->paired_state;
  animation = &rider_state->animation;
  speed = rider->velocity.forward;
  player = (RacerEntity *)race_rider_state.player_object;

  if(rider == player &&
     (rider_state->status_flags & RIDER_STATUS_EVENT_FLAG) == 0)
    {
      challenge_allowed = 1;
      if(rider->rider_mode == RIDER_MODE_STOPPED)
        {
          challenge_allowed = absolute_int_difference(speed, 0) <
                              CHALLENGE_MODE_SPEED_LIMIT;
        }
      else if(speed >= CHALLENGE_MAX_SPEED)
        {
          challenge_allowed = 0;
        }

      if(challenge_allowed)
        {
          candidate = find_nearby_active_challenge_racer(player);
          if(candidate != 0 &&
             absolute_int_difference(
               candidate->position_z - rider_state->position_z,
               0) < CHALLENGE_TRACK_DISTANCE &&
             absolute_int_difference(
               candidate->velocity.forward - speed,
               0) < CHALLENGE_SPEED_DISTANCE)
            {
              schedule_race_outcome_event(CHALLENGE_PROXIMITY_EVENT,
                                          CHALLENGE_PROXIMITY_VALUE);
            }
        }

      challenge_rider = (RacerEntity *)race_rider_state.challenge_object;
      if((CURRENT_FRAME_TICK << CHALLENGE_SPAWN_FRAME_SHIFT) == 0 &&
         challenge_rider == 0)
        {
          challenge_lane = move_road_lane_by_offset(
            player->track_segment,
            CHALLENGE_SELECTION_THRESHOLD);
          spawn_challenge_rider_at_lane(challenge_lane);
          challenge_rider = (RacerEntity *)race_rider_state.challenge_object;
          if(challenge_rider != 0)
            {
              challenge_rider->velocity.forward = 0;
            }
        }
    }

  animation_state =
    (signed char)animation->primary_state;
  if(rider->rider_mode == RIDER_MODE_NORMAL)
    {
      if(animation_state == RIDER_ANIMATION_CRUISE ||
         animation_state == -1 ||
         animation_state == RIDER_ANIMATION_STOPPED ||
         animation_state == RIDER_ANIMATION_FAST ||
         animation_state == RIDER_ANIMATION_LOW_SPEED)
        {
          if(speed < RIDER_LOW_SPEED_THRESHOLD)
            {
              select_primary(animation, RIDER_ANIMATION_STOPPED);
            }
          else if(speed < RIDER_MEDIUM_SPEED_THRESHOLD &&
                  rider->drive_acceleration >
                  RIDER_CONTROL_THRESHOLD)
            {
              select_primary(animation,
                                   RIDER_ANIMATION_LOW_SPEED);
            }
          else if((rider->action_flags & RIDER_ACTION_FLAG) != 0 &&
                  rider->velocity.vertical <
                  RIDER_ACTION_VALUE_LIMIT)
            {
              select_primary(animation, RIDER_ANIMATION_CRUISE);
            }
          else if(rider->reserved_100 <
                  RIDER_STEERING_THRESHOLD)
            {
              select_primary(animation, RIDER_ANIMATION_FAST);
            }
          else if(speed <= RIDER_FAST_SPEED_THRESHOLD)
            {
              select_primary(animation, RIDER_ANIMATION_CRUISE);
            }
          else
            {
              select_primary(animation, RIDER_ANIMATION_FAST);
            }
        }

      animation_state =
        (signed char)animation->primary_state;
      if(rider == player)
        {
          if((rider->status_flags & RIDER_STATUS_EVENT_FLAG) != 0 &&
             (((unsigned int)race_rider_state.status >>
               RACE_STATUS_SHIFT) < ACTIVE_RACE_STATUS_COUNT))
            {
              if(animation_state == RIDER_ANIMATION_EVENT)
                {
                  select_secondary(animation,
                                         RIDER_ANIMATION_EVENT);
                }
              else
                {
                  select_primary(animation, RIDER_ANIMATION_EVENT);
                }
            }
          if(animation->primary_state <
             RIDER_ANIMATION_CRUISE)
            {
              select_primary(animation, RIDER_ANIMATION_CRUISE);
            }
        }
      else
        {
          if(animation_state == RIDER_ANIMATION_TRANSITION &&
             animation->cycle_count >
             RIDER_ANIMATION_CRUISE)
            {
              select_primary(animation, RIDER_ANIMATION_CRUISE);
            }
          animation_state =
            (signed char)animation->primary_state;
          if(animation_state == RIDER_ANIMATION_IDLE ||
             animation_state == RIDER_ANIMATION_DISTANT ||
             animation_state == RIDER_ANIMATION_CRUISE)
            {
              value = absolute_int_difference(
                rider->position_z,
                track_simulation_globals.motion_source
                .packed_track_position);
              if(value <= RIDER_NEAR_DISTANCE)
                {
                  select_primary(animation, RIDER_ANIMATION_CRUISE);
                }
              else if(value <= RIDER_MEDIUM_DISTANCE)
                {
                  select_primary(animation, RIDER_ANIMATION_IDLE);
                }
              else
                {
                  select_primary(animation, RIDER_ANIMATION_DISTANT);
                }
            }
        }
    }
  else if(rider->rider_mode == RIDER_MODE_RECOVERY)
    {
      speed = rider_state->velocity.forward;
      if(speed == 0)
        {
          rider->rider_mode = RIDER_MODE_STOPPED;
          select_primary(animation, RIDER_ANIMATION_RECOVERY_STOP);
          select_secondary(animation, RIDER_ANIMATION_RECOVERY_END);
        }
      else
        {
          animation_state =
            (signed char)animation->primary_state;
          if(animation_state == RIDER_ANIMATION_RECOVERY_ALTERNATE ||
             animation_state == RIDER_ANIMATION_RECOVERY_TRIGGER)
            {
              if(rider_state->position_y <
                 RIDER_RECOVERY_VALUE_LIMIT)
                {
                  if(animation_state == RIDER_ANIMATION_RECOVERY_TRIGGER)
                    {
                      select_primary(
                        animation, RIDER_ANIMATION_RECOVERY_TRANSITION);
                      if(speed > RIDER_RECOVERY_FAST_SPEED)
                        {
                          select_secondary(
                            animation, RIDER_ANIMATION_RECOVERY_B);
                        }
                      else
                        {
                          random = rand();
                          select_secondary(
                            animation,
                            (random & RANDOM_CHOICE_MASK) == 0
                                    ? RIDER_ANIMATION_RECOVERY_C
                                    : RIDER_ANIMATION_RECOVERY_A);
                        }
                    }
                  else
                    {
                      select_primary(animation,
                                           RIDER_ANIMATION_RECOVERY_B);
                      select_secondary(animation,
                                             RIDER_ANIMATION_RECOVERY_B);
                    }
                }
            }
          else if(animation_state !=
                  RIDER_ANIMATION_RECOVERY_TRANSITION)
            {
              if(speed > RIDER_RECOVERY_CRUISE_SPEED)
                {
                  if(animation_state == RIDER_ANIMATION_RECOVERY_A ||
                     animation_state == RIDER_ANIMATION_RECOVERY_B)
                    {
                      select_secondary(
                        animation, RIDER_ANIMATION_RECOVERY_A);
                    }
                  else
                    {
                      select_primary(animation,
                                           RIDER_ANIMATION_RECOVERY_B);
                    }
                }
              else if(speed < RIDER_RECOVERY_SLOW_SPEED)
                {
                  select_primary(animation,
                                       RIDER_ANIMATION_RECOVERY_STOP);
                }
              else
                {
                  random = rand();
                  if((random & RANDOM_CHOICE_MASK) == 0 ||
                     animation_state == RIDER_ANIMATION_RECOVERY_A ||
                     animation_state == RIDER_ANIMATION_RECOVERY_B)
                    {
                      random = rand() & RANDOM_CHOICE_MASK;
                      if(random < RANDOM_CHOICE_MIDDLE)
                        {
                          select_secondary(
                            animation, RIDER_ANIMATION_RECOVERY_C);
                        }
                      else if(random == RANDOM_CHOICE_MIDDLE)
                        {
                          select_secondary(
                            animation, RIDER_ANIMATION_RECOVERY_D);
                        }
                      else
                        {
                          select_secondary(
                            animation, RIDER_ANIMATION_RECOVERY_E);
                        }
                    }
                  else
                    {
                      select_secondary(animation, animation_state);
                    }
                }
            }
        }
    }

  return advance_car_animation(animation);
}


static
int
current_player_profile_level(void)
{
  int player_index;

  player_index = race_mode_state.active_player_index;
  return player_profiles[player_index].level;
}


static
int
player_slot(void)
{
  RacerEntity *player;
  int position_slot;

  player = (RacerEntity *)race_rider_state.player_object;
  if(player == 0)
    {
      position_slot = PLAYER_SLOT_DEFAULT;
    }
  else
    {
      position_slot = player->paired_state->race_rank;
    }
  if(position_slot < PLAYER_SLOT_MINIMUM)
    {
      return PLAYER_SLOT_MINIMUM - position_slot;
    }
  return 0;
}


static
int
candidate_metric(RiderInteractionRuntime *racer_data,
                 RacerEntity             *candidate)
{
  RiderInteractionRuntime *candidate_data;
  RiderInteractionMetric *metric_entry;
  int metric_index;

  candidate_data = candidate->paired_state->interaction_data;
  metric_index = candidate_data->metric_index;
  if(metric_index >= RACER_METRIC_ENTRY_COUNT)
    {
      return 0;
    }
  metric_entry = &racer_data->current_metrics[metric_index];
  return metric_entry->components[RACER_METRIC_CURRENT_B] +
         metric_entry->components[RACER_METRIC_CURRENT_C] -
         metric_entry->components[RACER_METRIC_CURRENT_A];
}


static
int
classify_candidate(RiderInteractionRuntime *racer_data,
                   RacerEntity             *candidate)
{
  int metric;
  int profile;
  int slot;

  if(candidate == 0)
    {
      return 0;
    }
  metric = candidate_metric(racer_data, candidate);
  profile = current_player_profile_level();
  slot = player_slot();
  if(metric < CANDIDATE_LOW_BASE -
     profile * CANDIDATE_LOW_PROFILE_STEP -
     slot * CANDIDATE_LOW_SLOT_STEP)
    {
      return CANDIDATE_CLASS_LOW;
    }
  if(metric < CANDIDATE_MEDIUM_BASE -
     profile * CANDIDATE_MEDIUM_PROFILE_STEP -
     slot * CANDIDATE_MEDIUM_SLOT_STEP)
    {
      return CANDIDATE_CLASS_MEDIUM;
    }
  return CANDIDATE_CLASS_HIGH;
}


static
void
move_toward(int *value,
                  int  target,
                  int  step)
{
  if(*value > target)
    {
      *value -= step;
      if(*value < target)
        {
          *value = target;
        }
    }
  else if(*value < target)
    {
      *value += step;
      if(*value > target)
        {
          *value = target;
        }
    }
}


static
int
divide(int numerator,
             int denominator)
{
  SignedDivisionResult result;

  result = signed_divide_with_remainder((unsigned int)denominator,
                                        (unsigned int)numerator);
  return (int)result.quotient;
}


static
int
div4_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_FOUR_BIAS;
    }
  return value >> DIVIDE_BY_FOUR_SHIFT;
}


static
int
div8_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_EIGHT_BIAS;
    }
  return value >> DIVIDE_BY_EIGHT_SHIFT;
}


static
int
div16_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_SIXTEEN_BIAS;
    }
  return value >> DIVIDE_BY_SIXTEEN_SHIFT;
}


static
int
div32_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_THIRTY_TWO_BIAS;
    }
  return value >> DIVIDE_BY_THIRTY_TWO_SHIFT;
}


static
int
div2_toward_zero(int value)
{
  if(value < 0)
    {
      value += 1;
    }
  return value >> DIVIDE_BY_TWO_SHIFT;
}


void
update_rider_pacing_state(char *rider)
{
  RacerEntity *rider_entity;
  RacerEntity *rider_state;
  RiderInteractionRuntime *racer_data;
  RacerEntity *challenge_rider;
  RacerEntity *player;
  RacerEntity *left_rider;
  RacerEntity *right_rider;
  RacerEntity *selected_rider;
  int *metrics;
  int left_class;
  int right_class;
  int selected_class;
  int search_range;
  int is_challenge;
  int profile_level;
  int pacing_value;
  int pacing_factor;
  int player_delta;
  int candidate_presence;
  int metric_index;

  rider_entity = (RacerEntity *)rider;
  rider_state = rider_entity->paired_state;
  racer_data = rider_state->interaction_data;
  challenge_rider = (RacerEntity *)race_rider_state.challenge_object;
  is_challenge = challenge_rider != 0 &&
                 rider_state == challenge_rider->paired_state;
  search_range = is_challenge ? SPECIAL_RIDER_RANGE : NORMAL_RIDER_RANGE;

  while(racer_data->pacing_frame_accumulator >= RACER_UPDATE_PERIOD)
    {
      if(is_challenge)
        {
          racer_data->target_speed_limit -= 1;
          if(racer_data->target_speed_limit <
             PACING_SPECIAL_COUNT_LIMIT)
            {
              profile_level = current_player_profile_level();
              racer_data->target_speed_limit =
                racer_data->pacing_values[profile_level];
            }
        }

      metric_index = 0;
      while(metric_index < RACER_METRIC_ENTRY_COUNT)
        {
          metrics = (int *)&racer_data->current_metrics[metric_index];
          move_toward(
            &metrics[RACER_METRIC_CURRENT_A],
            metrics[RACER_METRIC_TARGET_A],
            *(int *)((char *)racer_data +
                     RACER_DATA_ADJUSTMENT_STEP_A_OFFSET));
          move_toward(
            &metrics[RACER_METRIC_CURRENT_B],
            metrics[RACER_METRIC_TARGET_B],
            *(int *)((char *)racer_data +
                     RACER_DATA_ADJUSTMENT_STEP_B_OFFSET));
          move_toward(
            &metrics[RACER_METRIC_CURRENT_C],
            metrics[RACER_METRIC_TARGET_C],
            *(int *)((char *)racer_data +
                     RACER_DATA_ADJUSTMENT_STEP_C_OFFSET));
          metric_index++;
        }
      racer_data->pacing_frame_accumulator -= RACER_UPDATE_PERIOD;
    }

  left_rider = find_nearby_pacing_rider_in_direction(
    rider_entity, 1, search_range);
  right_rider = find_nearby_pacing_rider_in_direction(
    rider_entity, 0, search_range);
  racer_data->left_candidate = left_rider;
  racer_data->right_candidate = right_rider;

  left_class = classify_candidate(racer_data, left_rider);
  right_class = classify_candidate(racer_data, right_rider);
  racer_data->left_candidate_class = left_class;
  racer_data->right_candidate_class = right_class;

  if((unsigned int)left_class > (unsigned int)right_class)
    {
      selected_rider = left_rider;
      selected_class = left_class;
    }
  else if((unsigned int)right_class > (unsigned int)left_class)
    {
      selected_rider = right_rider;
      selected_class = right_class;
    }
  else if(right_rider == 0)
    {
      selected_rider = left_rider;
      selected_class = left_class;
    }
  else if(left_rider == 0)
    {
      selected_rider = right_rider;
      selected_class = right_class;
    }
  else if(absolute_int_difference(
            rider_entity->position_z - left_rider->position_z,
            0) <=
          absolute_int_difference(
            rider_entity->position_z - right_rider->position_z,
            0))
    {
      selected_rider = left_rider;
      selected_class = left_class;
    }
  else
    {
      selected_rider = right_rider;
      selected_class = right_class;
    }
  racer_data->attack_target = selected_rider;
  racer_data->attack_target_class = selected_class;

  if(is_challenge)
    {
      return;
    }

  profile_level = current_player_profile_level();
  pacing_value = racer_data->pacing_values[profile_level];
  racer_data->target_speed_limit = pacing_value;
  racer_data->target_acceleration =
    racer_data->profile_values[profile_level];

  player = (RacerEntity *)race_rider_state.player_object;
  if(player != 0 &&
     (left_rider == player || right_rider == player))
    {
      player_delta =
        (rider_entity->position_z - player->position_z) >>
        PACING_DELTA_SHIFT;
      if(player_delta < PACING_DELTA_MIN)
        {
          player_delta = PACING_DELTA_MIN;
        }
      else if(player_delta > PACING_DELTA_MAX)
        {
          player_delta = PACING_DELTA_MAX;
        }
      pacing_factor =
        rider_pacing_factor_adjustments[player_delta +
                                        PACING_DELTA_INDEX_BIAS] +
        PACING_PERCENT_BASE;
      pacing_value = divide(
        pacing_value * pacing_factor, PACING_PERCENT_BASE);
    }
  else if(selected_rider != 0 &&
          selected_class == CANDIDATE_CLASS_HIGH)
    {
      pacing_value += div8_toward_zero(pacing_value);
    }
  else
    {
      candidate_presence = (right_rider != 0 ? 1 : 0) |
                           (left_rider != 0 ? PACING_LEFT_PRESENT : 0);
      if(candidate_presence == 0)
        {
          if(player == 0 ||
             rider_entity->position_z <= player->position_z)
            {
              pacing_value += div4_toward_zero(pacing_value);
            }
          else
            {
              pacing_value = divide(
                pacing_value *
                (PACING_PERCENT_BASE -
                 PACING_LEFT_PRESENT *
                 (PACING_PROFILE_SCALE - profile_level)),
                PACING_PERCENT_BASE);
            }
        }
      else if(candidate_presence == 1)
        {
          pacing_value += div16_toward_zero(pacing_value);
        }
      else if(candidate_presence == PACING_LEFT_PRESENT)
        {
          pacing_value = divide(
            pacing_value *
            (PACING_PERCENT_BASE -
             (PACING_PROFILE_SCALE - profile_level)),
            PACING_PERCENT_BASE);
        }
      else if(player == 0 ||
              rider_entity->position_z <= player->position_z)
        {
          pacing_value += div32_toward_zero(pacing_value);
        }
      else
        {
          pacing_value = divide(
            pacing_value *
            (PACING_PERCENT_BASE -
             (PACING_PROFILE_SCALE - profile_level) /
             PACING_LEFT_PRESENT),
            PACING_PERCENT_BASE);
        }
    }
  racer_data->target_speed_limit = pacing_value;

  if(player != 0 &&
     player->rider_mode != RIDER_MODE_NORMAL &&
     absolute_int_difference(
       player->position_z,
       rider_entity->position_z) >
     RIDER_MEDIUM_DISTANCE)
    {
      profile_level = current_player_profile_level();
      if(profile_level == PACING_SPECIAL_PROFILE)
        {
          pacing_value = div4_toward_zero(
            pacing_value * PACING_SPECIAL_PROFILE);
        }
      else if(profile_level != PACING_EXCLUDED_PROFILE)
        {
          pacing_value = div2_toward_zero(pacing_value);
        }
      racer_data->target_speed_limit = pacing_value;
    }
}


#undef CURRENT_FRAME_TICK

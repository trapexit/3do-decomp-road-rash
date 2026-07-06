#include "ai_collision_avoidance_runtime.h"

#include "division_helpers.h"
#include "rider_contact_runtime.h"
#include "spatial_audio_event_queue.h"
#include "track_traversal_runtime.h"

enum RiderAttackApproachConstant
{
  RIDER_CHALLENGE_TARGET_LOST_EVENT = 0x16,
  RIDER_DISTANCE_PROJECTION_SHIFT = 4,
  RIDER_CHALLENGE_DISTANCE_LIMIT = 0x1F4,
  RIDER_NORMAL_DISTANCE_LIMIT = 0xA0,
  RIDER_COLLISION_LENGTH_SCALE_SHIFT = 2,
  RIDER_APPROACH_OFFSET_BASE = 0x4650,
  RIDER_APPROACH_OFFSET_MAXIMUM = 0x8CA0,
  RIDER_APPROACH_OFFSET_DISTANCE_SHIFT = 12,
  RIDER_LANE_WIDTH_DIVISOR = 0xFA00,
  RIDER_ATTACK_TRACK_LEAD = 0x3C0,
  RIDER_SPEED_PRIMARY_SHIFT = 7,
  RIDER_SPEED_SECONDARY_SHIFT = 9,
  RIDER_SPEED_TERTIARY_SHIFT = 12,
  RIDER_SPEED_APPROACH_STEP = 5,
  RIDER_SPEED_POSITION_BIAS = 1,
  RIDER_SPEED_POSITION_MODE = 0,
  RIDER_SPEED_MATCH_LIMIT = 0x7D0,
  RIDER_SPEED_DIRECT_MINIMUM = 0xFA0,
  RIDER_SPEED_RESULT_SCALE = 100,
  RIDER_TARGET_MODE_RECOVERY = 1,
  RIDER_CHALLENGE_SPEED_CAP = 0x14
};

int
rider_update_mode_1(int rider_address);

static
int
truncate_track_width(int value)
{
  if(value < 0)
    {
      value += TRACK_POSITION_FRACTION_MASK;
    }
  return value >> TRACK_POSITION_FRACTION_BITS;
}


static
int
rider_speed_units(int value)
{
  return (value >> RIDER_SPEED_PRIMARY_SHIFT) +
         (value >> RIDER_SPEED_SECONDARY_SHIFT) +
         (value >> RIDER_SPEED_TERTIARY_SHIFT);
}


void
rider_update_mode_4(int rider_address)
{
  RacerEntity *rider;
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  RacerEntity *target;
  RacerEntity *target_state;
  TrackObjectPosition *current_position;
  TrackObjectPosition *target_position;
  RacerEntity *challenge;
  SignedDivisionResult division;
  int signed_distance;
  int distance;
  int scaled_distance;
  int distance_limit;
  int target_lateral_position;
  int lateral_offset;
  int lane_offset;
  int lane_bound;
  int target_speed;
  int rider_speed;
  int wanted_speed;
  int speed_difference;
  int target_mode;

  rider = (RacerEntity *)rider_address;
  state = rider->paired_state;
  interaction = state->interaction_data;
  target = interaction->attack_target;
  challenge = (RacerEntity *)race_rider_state.challenge_object;

  if(target == 0)
    {
      if(challenge != 0 && challenge->paired_state == state)
        {
          queue_spatial_audio_event(
            RIDER_CHALLENGE_TARGET_LOST_EVENT, 0, 0);
        }
      rider_update_mode_1(rider_address);
      return;
    }

  target_state = target->paired_state;
  current_position = (TrackObjectPosition *)&rider->position_z;
  target_position = (TrackObjectPosition *)&target_state->position_z;

  signed_distance = target_position->track_position -
                    current_position->track_position;
  if(signed_distance <= 0)
    {
      distance = -signed_distance;
    }
  else
    {
      distance = signed_distance;
    }
  distance >>= RIDER_DISTANCE_PROJECTION_SHIFT;

  if(challenge != 0 && challenge->paired_state == state)
    {
      distance_limit = RIDER_CHALLENGE_DISTANCE_LIMIT;
    }
  else
    {
      distance_limit = RIDER_NORMAL_DISTANCE_LIMIT;
    }

  if(signed_distance > 0 && distance > distance_limit)
    {
      rider_update_mode_1(rider_address);
      return;
    }

  if(select_ai_collision_avoidance_target(
       (AiRacerObject *)rider) == 0)
    {
      scaled_distance = distance << RIDER_DISTANCE_PROJECTION_SHIFT;
      if(scaled_distance <
         (rider->collision_half_length_z <<
          RIDER_COLLISION_LENGTH_SCALE_SHIFT))
        {
          lateral_offset = RIDER_APPROACH_OFFSET_BASE +
                           (distance <<
                            RIDER_APPROACH_OFFSET_DISTANCE_SHIFT);
          if(lateral_offset > RIDER_APPROACH_OFFSET_MAXIMUM)
            {
              lateral_offset = RIDER_APPROACH_OFFSET_MAXIMUM;
            }

          target_lateral_position = target_position->lateral_position;
          lane_offset = target_lateral_position + lateral_offset -
                        (target_state->track_segment->center_position <<
                         TRACK_POSITION_FRACTION_BITS);

          if(rider->position_x > target_lateral_position)
            {
              division = signed_divide_with_remainder(
                RIDER_LANE_WIDTH_DIVISOR,
                (unsigned int)lane_offset);
              lane_bound = division.quotient;
              if(lane_offset < 0)
                {
                  lane_bound--;
                }

              if(lane_bound >= 0 &&
                 lane_bound < truncate_track_width(
                   target_state->track_segment->right_width))
                {
                  lateral_offset = -lateral_offset;
                }
            }

          interaction->target_track_position =
            target_position->track_position + RIDER_ATTACK_TRACK_LEAD;
          interaction->target_lateral_position =
            target_lateral_position - lateral_offset;
        }
      else
        {
          select_ai_curvature_lane_target((AiRacerObject *)rider);
        }

      target_speed = target_state->velocity.forward;
      rider_speed = rider->velocity.forward;
      wanted_speed = approach_track_object_speed(
        current_position, rider_speed_units(rider_speed),
        interaction->target_speed_limit,
        interaction->target_acceleration, target_position,
        rider_speed_units(target_speed), RIDER_SPEED_APPROACH_STEP,
        distance_limit, RIDER_SPEED_POSITION_BIAS,
        RIDER_SPEED_POSITION_MODE);
      interaction->target_speed = wanted_speed;

      speed_difference = target_speed - rider_speed;
      if(speed_difference <= 0)
        {
          speed_difference = -speed_difference;
        }

      if(speed_difference < RIDER_SPEED_MATCH_LIMIT &&
         scaled_distance <
         (rider->collision_half_length_z <<
          RIDER_COLLISION_LENGTH_SCALE_SHIFT) &&
         rider_speed > RIDER_SPEED_DIRECT_MINIMUM)
        {
          rider->velocity.forward =
            wanted_speed * RIDER_SPEED_RESULT_SCALE;
        }
      else
        {
          challenge = (RacerEntity *)race_rider_state.challenge_object;
          target = interaction->attack_target;
          if(challenge != 0 && challenge->paired_state == state &&
             target != 0)
            {
              target_mode = target->rider_mode;
              if(target_mode == RIDER_TARGET_MODE_RECOVERY)
                {
                  interaction->target_speed = 0;
                }
              else if(target_mode != 0)
                {
                  if(wanted_speed > RIDER_CHALLENGE_SPEED_CAP)
                    {
                      wanted_speed = RIDER_CHALLENGE_SPEED_CAP;
                    }
                  interaction->target_speed = wanted_speed;
                }
            }
        }
    }

  apply_ai_rider_navigation_control(rider);
}

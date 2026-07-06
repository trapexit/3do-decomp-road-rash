#include "ai_collision_avoidance_runtime.h"
#include "division_helpers.h"

enum AiCollisionAvoidanceValue
{
  AI_AVOIDANCE_OBJECT_MASK = 0x3F,
  AI_AVOIDANCE_SEARCH_MODE = 0,
  AI_AVOIDANCE_MAXIMUM_INTERCEPT_TIME = 0x96,
  AI_AVOIDANCE_MAXIMUM_SEARCH_DISTANCE = 0x960,
  AI_AVOIDANCE_LATERAL_WINDOW = 0xC000,
  AI_AVOIDANCE_LATERAL_CLEARANCE = 0x5000,
  AI_AVOIDANCE_LANE_SPACING = 0xFA00,
  AI_AVOIDANCE_TARGET_TRACK_LEAD = 0x3C0,
  AI_AVOIDANCE_POSITION_FRACTION_BITS = 8,
  AI_AVOIDANCE_DIVIDE_256_BIAS = 0xFF,
  AI_AVOIDANCE_DIVIDE_FOUR_BIAS = 3,
  AI_AVOIDANCE_DIVIDE_FOUR_SHIFT = 2,
  AI_AVOIDANCE_SIGN_SHIFT = 31,
  AI_AVOIDANCE_IMMEDIATE_TIME = 0x25,
  AI_AVOIDANCE_NEAR_TRACK_DISTANCE = 0x320,
  AI_AVOIDANCE_NEAR_ANGLE = 0x0F,
  AI_AVOIDANCE_APPROACH_TIME = 0x4B
};

static
int
ai_avoidance_half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >>
                AI_AVOIDANCE_SIGN_SHIFT)) >> 1;
}


static
int
ai_avoidance_quarter_towards_zero(int value)
{
  if(value < 0)
    {
      value += AI_AVOIDANCE_DIVIDE_FOUR_BIAS;
    }
  return value >> AI_AVOIDANCE_DIVIDE_FOUR_SHIFT;
}


static
int
ai_avoidance_divide_256_towards_zero(int value)
{
  if(value < 0)
    {
      value += AI_AVOIDANCE_DIVIDE_256_BIAS;
    }
  return value >> AI_AVOIDANCE_POSITION_FRACTION_BITS;
}


static
int
ai_avoidance_absolute_value(int value)
{
  if(value <= 0)
    {
      value = -value;
    }
  return value;
}


int
select_ai_collision_avoidance_target(AiRacerObject *racer)
{
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  TrackInteractionObject *candidate;
  RoadSegmentLaneRuntime *candidate_lane;
  SignedDivisionResult division;
  int intercept_time;
  int lateral_step;
  int lateral_target;
  int lane_delta;
  int lane_sample;
  int lane_bound;
  int track_distance;
  int bearing;
  int speed_target;

  state = (RacerEntity *)racer->paired_state;
  interaction = state->interaction_data;

  candidate = find_collision_intercept_target(
    (TrackInteractionObject *)racer,
    (TrackInteractionObject *)state,
    AI_AVOIDANCE_OBJECT_MASK,
    AI_AVOIDANCE_SEARCH_MODE,
    AI_AVOIDANCE_MAXIMUM_INTERCEPT_TIME,
    AI_AVOIDANCE_MAXIMUM_SEARCH_DISTANCE,
    AI_AVOIDANCE_LATERAL_WINDOW,
    &intercept_time);
  interaction->avoidance_target = candidate;
  if(candidate == 0 ||
     candidate == (TrackInteractionObject *)interaction->attack_target)
    {
      interaction->avoidance_target = 0;
      return 0;
    }

  lateral_step = ai_avoidance_half_towards_zero(
    collision_avoidance_lateral_width(candidate) +
    collision_avoidance_lateral_width(
      (TrackInteractionObject *)racer));
  lateral_step += AI_AVOIDANCE_LATERAL_CLEARANCE;

  lateral_target = candidate->position.lateral_position;
  if(racer->position.lateral_position <= lateral_target)
    {
      lateral_step = -lateral_step;
    }

  candidate_lane = candidate->track_segment;
  lane_delta = lateral_target + lateral_step -
               (candidate_lane->center_position <<
                AI_AVOIDANCE_POSITION_FRACTION_BITS);
  division = signed_divide_with_remainder(
    AI_AVOIDANCE_LANE_SPACING, lane_delta);
  lane_sample = division.quotient;
  if(lane_delta < 0)
    {
      lane_sample -= 1;
    }
  lane_sample = (signed char)lane_sample;

  lane_bound = ai_avoidance_divide_256_towards_zero(
    candidate_lane->right_width);
  if(lane_bound <= lane_sample)
    {
      lateral_step = -lateral_step;
      no_op_return();
    }
  else
    {
      lane_bound = -ai_avoidance_divide_256_towards_zero(
        candidate_lane->left_width);
      if(lane_bound > lane_sample)
        {
          lateral_step = -lateral_step;
          no_op_return();
        }
    }

  lateral_target += lateral_step;
  interaction->target_track_position =
    candidate->position.track_position +
    AI_AVOIDANCE_TARGET_TRACK_LEAD;
  interaction->target_lateral_position = lateral_target;

  track_distance = ai_avoidance_absolute_value(
    racer->position.track_position -
    candidate->position.track_position);
  if(intercept_time < AI_AVOIDANCE_IMMEDIATE_TIME ||
     track_distance < AI_AVOIDANCE_NEAR_TRACK_DISTANCE)
    {
      bearing = calculate_track_position_bearing(
        (const TrackObjectPosition *)&racer->position,
        &candidate->position);
      bearing = ai_avoidance_absolute_value(bearing);
      if(bearing < AI_AVOIDANCE_NEAR_ANGLE)
        {
          speed_target = calculate_ai_curve_speed_target(racer);
          interaction->target_speed =
            ai_avoidance_half_towards_zero(speed_target);
        }
    }
  else if(intercept_time < AI_AVOIDANCE_APPROACH_TIME)
    {
      speed_target = calculate_ai_curve_speed_target(racer);
      interaction->target_speed =
        speed_target -
        ai_avoidance_quarter_towards_zero(speed_target);
    }
  else
    {
      interaction->target_speed =
        calculate_ai_curve_speed_target(racer);
    }

  return 1;
}

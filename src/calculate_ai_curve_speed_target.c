#include "ai_collision_avoidance_runtime.h"
#include "division_helpers.h"
#include "road_segment_runtime.h"

enum AiCurveSpeedValue
{
  AI_CURVE_LOOKAHEAD_DISTANCE = 0x800,
  AI_CURVE_SAMPLE_COUNT = 4,
  AI_CURVE_LIMIT_REDUCTION_BIAS = 3,
  AI_CURVE_LIMIT_REDUCTION_SHIFT = 2,
  AI_CURVE_SIGN_SHIFT = 31,
  AI_CURVE_FRACTION_BITS = 8,
  AI_CURVE_SLOWDOWN_THRESHOLD = 0x30000,
  AI_CURVE_RESPONSE_DIVISOR = 0x600
};

static
int
ai_curve_absolute_value(int value)
{
  if(value <= 0)
    {
      value = -value;
    }
  return value;
}


static
int
ai_curve_half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> AI_CURVE_SIGN_SHIFT)) >> 1;
}


int
calculate_ai_curve_speed_target(AiRacerObject *racer)
{
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  RoadSegmentLaneRuntime *lookahead_lane;
  SignedDivisionResult division;
  int curvature;
  int speed_limit;
  int reduced_limit;
  int reduction_divisor;
  int scaled_curvature;
  int half_limit;

  state = (RacerEntity *)racer->paired_state;
  interaction = state->interaction_data;

  lookahead_lane = move_road_lane_by_offset(
    racer->track_segment, AI_CURVE_LOOKAHEAD_DISTANCE);
  curvature = ai_curve_absolute_value(
    average_road_lane_curvature(
      lookahead_lane, AI_CURVE_SAMPLE_COUNT));

  speed_limit = interaction->target_speed_limit;
  reduction_divisor = speed_limit;
  if(reduction_divisor < 0)
    {
      reduction_divisor += AI_CURVE_LIMIT_REDUCTION_BIAS;
    }
  reduced_limit = speed_limit -
                  (reduction_divisor >>
                   AI_CURVE_LIMIT_REDUCTION_SHIFT);

  if(curvature <= AI_CURVE_SLOWDOWN_THRESHOLD)
    {
      return speed_limit;
    }

  scaled_curvature = curvature >> AI_CURVE_FRACTION_BITS;
  half_limit = ai_curve_half_towards_zero(reduced_limit);
  division = signed_divide_with_remainder(
    AI_CURVE_RESPONSE_DIVISOR,
    half_limit << AI_CURVE_FRACTION_BITS);

  return reduced_limit -
         ((division.quotient * scaled_curvature) >>
          AI_CURVE_FRACTION_BITS) +
         half_limit;
}

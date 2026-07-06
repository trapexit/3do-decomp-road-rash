#include "ai_collision_avoidance_runtime.h"
#include "division_helpers.h"

enum AiCurvatureLaneTargetValue
{
  AI_CURVATURE_LANE_TRACK_LEAD = 0x3C0,
  AI_CURVATURE_LANE_SAMPLE_COUNT = 8,
  AI_CURVATURE_LANE_SIGN_SHIFT = 31,
  AI_CURVATURE_LANE_FRACTION_BITS = 8,
  AI_CURVATURE_SPEED_SHIFT_A = 7,
  AI_CURVATURE_SPEED_SHIFT_B = 9,
  AI_CURVATURE_SPEED_SHIFT_C = 12,
  AI_CURVATURE_DIRECT_TARGET_THRESHOLD = 0x30000,
  AI_CURVATURE_MAXIMUM = 0x60000
};

static
int
ai_curvature_absolute_value(int value)
{
  if(value <= 0)
    {
      value = -value;
    }
  return value;
}


static
int
ai_curvature_half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >>
                AI_CURVATURE_LANE_SIGN_SHIFT)) >> 1;
}


int
select_ai_curvature_lane_target(AiRacerObject *racer)
{
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  RoadSegmentLaneRuntime *lane;
  SignedDivisionResult division;
  int wanted_track_position;
  int previous_track_position;
  int curvature;
  int curvature_magnitude;
  int speed_limit;
  int curve_speed;
  int lane_half_width;
  int lane_center;
  int curve_factor;
  int lateral_target;
  int lateral_clamp;

  state = (RacerEntity *)racer->paired_state;
  interaction = state->interaction_data;

  wanted_track_position =
    AI_CURVATURE_LANE_TRACK_LEAD +
    (racer->position.track_position >>
     AI_CURVATURE_LANE_FRACTION_BITS);
  previous_track_position = interaction->target_track_position;
  if(wanted_track_position ==
     (previous_track_position >>
      AI_CURVATURE_LANE_FRACTION_BITS))
    {
      return previous_track_position;
    }

  lane = racer->track_segment;
  curvature = average_road_lane_curvature(
    lane, AI_CURVATURE_LANE_SAMPLE_COUNT);
  curvature_magnitude = ai_curvature_absolute_value(curvature);
  if(curvature_magnitude <
     AI_CURVATURE_DIRECT_TARGET_THRESHOLD)
    {
      return select_ai_lane_target(racer);
    }

  speed_limit =
    (racer->velocity.forward >> AI_CURVATURE_SPEED_SHIFT_A) +
    (racer->velocity.forward >> AI_CURVATURE_SPEED_SHIFT_B) +
    (racer->velocity.forward >> AI_CURVATURE_SPEED_SHIFT_C);

  curve_speed = calculate_ai_curve_speed_target(racer);
  if(ai_curvature_half_towards_zero(curve_speed) > speed_limit)
    {
      return select_ai_lane_target(racer);
    }

  lane_half_width = ai_curvature_half_towards_zero(
    lane->right_width);
  lane_half_width <<= AI_CURVATURE_LANE_FRACTION_BITS;
  lane_center = (lane->left_width + lane->road_left) <<
                AI_CURVATURE_LANE_FRACTION_BITS;

  if(curvature_magnitude > AI_CURVATURE_MAXIMUM)
    {
      curvature_magnitude = AI_CURVATURE_MAXIMUM;
    }
  division = signed_divide_with_remainder(
    AI_CURVATURE_MAXIMUM,
    curvature_magnitude << AI_CURVATURE_LANE_FRACTION_BITS);
  curve_factor = division.quotient;
  lateral_target = (lane_half_width * curve_factor) >>
                   AI_CURVATURE_LANE_FRACTION_BITS;
  if(curvature < 0)
    {
      lateral_target = -lateral_target;
    }

  interaction->target_track_position =
    racer->position.track_position +
    AI_CURVATURE_LANE_TRACK_LEAD;
  lateral_target = lane_center + lane_half_width + lateral_target;
  interaction->target_lateral_position = lateral_target;
  interaction->target_speed = curve_speed;

  lateral_clamp = racer->position.lateral_position;
  if(curvature > 0)
    {
      if(lateral_clamp > lateral_target)
        {
          interaction->target_lateral_position = lateral_clamp;
        }
    }
  else if(lateral_clamp < lateral_target)
    {
      interaction->target_lateral_position = lateral_clamp;
    }

  return lateral_target;
}

#include "ai_collision_avoidance_runtime.h"

enum AiLaneTargetValue
{
  AI_LANE_TARGET_LINK_INDEX = 2,
  AI_LANE_MODE_CENTER = 0,
  AI_LANE_MODE_HALF_WIDTH = 1,
  AI_LANE_MODE_FULL_WIDTH = 2,
  AI_LANE_POSITION_FRACTION_BITS = 8,
  AI_LANE_POSITION_ROUNDING_BIAS = 0xFF,
  AI_LANE_CURVATURE_SAMPLE_COUNT = 8,
  AI_LANE_CURVATURE_ROUNDING_BIAS = 7,
  AI_LANE_CURVATURE_SHIFT = 3,
  AI_LANE_HALF_SIGN_SHIFT = 31,
  AI_LANE_SPACING = 250,
  AI_LANE_TARGET_TRACK_LEAD = 0x3C0
};

static
int
ai_lane_half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >>
                AI_LANE_HALF_SIGN_SHIFT)) >> 1;
}


static
int
ai_lane_divide_by_eight_towards_zero(int value)
{
  if(value < 0)
    {
      value += AI_LANE_CURVATURE_ROUNDING_BIAS;
    }
  return value >> AI_LANE_CURVATURE_SHIFT;
}


int
select_ai_lane_target(AiRacerObject *racer)
{
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  RoadSegmentLaneRuntime *target_lane;
  int mode;
  int lane_index;
  int lane_span;
  int curvature;
  int lateral_target;
  int speed_target;

  state = (RacerEntity *)racer->paired_state;
  interaction = state->interaction_data;
  target_lane = racer->track_segment
                ->previous_links[AI_LANE_TARGET_LINK_INDEX];

  mode = interaction->lane_selection_mode;
  if(mode == AI_LANE_MODE_CENTER)
    {
      lane_index = 0;
    }
  else if(mode == AI_LANE_MODE_HALF_WIDTH)
    {
      lane_span = target_lane->right_width;
      if(lane_span < 0)
        {
          lane_span += AI_LANE_POSITION_ROUNDING_BIAS;
        }
      lane_span =
        (lane_span >> AI_LANE_POSITION_FRACTION_BITS) - 1;
      lane_index = ai_lane_half_towards_zero(lane_span);
    }
  else if(mode == AI_LANE_MODE_FULL_WIDTH)
    {
      lane_span = target_lane->right_width;
      if(lane_span < 0)
        {
          lane_span += AI_LANE_POSITION_ROUNDING_BIAS;
        }
      lane_span =
        (lane_span >> AI_LANE_POSITION_FRACTION_BITS) - 1;
      lane_index = lane_span <= 0 ? -lane_span : lane_span;
    }
  else
    {
      lane_index = 0;
    }

  interaction->target_track_position =
    racer->position.track_position + AI_LANE_TARGET_TRACK_LEAD;

  curvature = average_road_lane_curvature(
    target_lane, AI_LANE_CURVATURE_SAMPLE_COUNT);
  lateral_target = target_lane->center_position +
                   lane_index * AI_LANE_SPACING;
  lateral_target = interaction->lane_center_bias +
                   (lateral_target <<
                    AI_LANE_POSITION_FRACTION_BITS);
  lateral_target +=
    ai_lane_divide_by_eight_towards_zero(curvature);
  interaction->target_lateral_position = lateral_target;

  speed_target = calculate_ai_curve_speed_target(racer);
  interaction->target_speed = speed_target;
  return speed_target;
}

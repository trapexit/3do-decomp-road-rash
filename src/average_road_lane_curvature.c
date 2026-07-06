#include "ai_collision_avoidance_runtime.h"
#include "division_helpers.h"

enum RoadLaneCurvatureValue
{
  ROAD_LANE_CURVATURE_BITS_PER_BYTE = 8,
  ROAD_LANE_CURVATURE_LINK_INDEX = 2,
  ROAD_LANE_CURVATURE_SIGN_BIT = 0x00800000,
  ROAD_LANE_CURVATURE_WRAP = 0x01000000,
  ROAD_LANE_CURVATURE_BYTE_MASK = 0xFF,
  ROAD_LANE_CURVATURE_SAMPLE_TWO = 2,
  ROAD_LANE_CURVATURE_SAMPLE_FOUR = 4,
  ROAD_LANE_CURVATURE_SAMPLE_EIGHT = 8,
  ROAD_LANE_CURVATURE_SAMPLE_SIXTEEN = 16,
  ROAD_LANE_CURVATURE_SHIFT_TWO = 1,
  ROAD_LANE_CURVATURE_SHIFT_FOUR = 2,
  ROAD_LANE_CURVATURE_SHIFT_EIGHT = 3,
  ROAD_LANE_CURVATURE_SHIFT_SIXTEEN = 4,
  ROAD_LANE_CURVATURE_BIAS_FOUR = 3,
  ROAD_LANE_CURVATURE_BIAS_EIGHT = 7,
  ROAD_LANE_CURVATURE_BIAS_SIXTEEN = 15
};

int
average_road_lane_curvature(RoadSegmentLaneRuntime *lane,
                            int                     sample_count)
{
  int divisor;
  int remaining;
  int sum;
  int sample;
  int count;

  divisor = (signed char)sample_count;
  remaining = divisor;
  sum = 0;

  while(lane != 0)
    {
      count = remaining;
      remaining = (signed char)(remaining - 1);
      if((count & ROAD_LANE_CURVATURE_BYTE_MASK) == 0)
        {
          break;
        }

      sample = lane->curvature_projection_step;
      if(sample >= ROAD_LANE_CURVATURE_SIGN_BIT)
        {
          sample -= ROAD_LANE_CURVATURE_WRAP;
        }
      sum += sample;
      lane = lane->previous_links[ROAD_LANE_CURVATURE_LINK_INDEX];
    }

  if(divisor == 0)
    {
      return 0;
    }
  if(divisor == 1)
    {
      return sum;
    }
  if(divisor == ROAD_LANE_CURVATURE_SAMPLE_TWO)
    {
      return (sum + (int)((unsigned int)sum >>
                          (sizeof(sum) *
                           ROAD_LANE_CURVATURE_BITS_PER_BYTE - 1))) >>
             ROAD_LANE_CURVATURE_SHIFT_TWO;
    }
  if(divisor == ROAD_LANE_CURVATURE_SAMPLE_FOUR)
    {
      if(sum < 0)
        {
          sum += ROAD_LANE_CURVATURE_BIAS_FOUR;
        }
      return sum >> ROAD_LANE_CURVATURE_SHIFT_FOUR;
    }
  if(divisor == ROAD_LANE_CURVATURE_SAMPLE_EIGHT)
    {
      if(sum < 0)
        {
          sum += ROAD_LANE_CURVATURE_BIAS_EIGHT;
        }
      return sum >> ROAD_LANE_CURVATURE_SHIFT_EIGHT;
    }
  if(divisor == ROAD_LANE_CURVATURE_SAMPLE_SIXTEEN)
    {
      if(sum < 0)
        {
          sum += ROAD_LANE_CURVATURE_BIAS_SIXTEEN;
        }
      return sum >> ROAD_LANE_CURVATURE_SHIFT_SIXTEEN;
    }

  return signed_divide_with_remainder(
    (unsigned int)divisor, (unsigned int)sum).quotient;
}

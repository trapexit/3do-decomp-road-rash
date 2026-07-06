#include "bss_late_data.h"

/* Reconstructed from the original function at 0x000213B4. */

enum RoadLaneWidthConstants
{
  ROAD_LANE_DEFAULT_WIDTH = 0x200
};

int
sample_road_lane_widths(const RoadRlanTraversalState *lane_state,
                        int                          *out_left_width,
                        int                          *out_right_width)
{
  if(lane_state->resource == 0)
    {
      *out_left_width = ROAD_LANE_DEFAULT_WIDTH;
      *out_right_width = ROAD_LANE_DEFAULT_WIDTH;
      return ROAD_LANE_DEFAULT_WIDTH;
    }

  *out_left_width =
    lane_state->left_width_accumulator >>
    TRACK_POSITION_FRACTION_BITS;
  *out_right_width =
    lane_state->right_width_accumulator >>
    TRACK_POSITION_FRACTION_BITS;
  return *out_right_width;
}

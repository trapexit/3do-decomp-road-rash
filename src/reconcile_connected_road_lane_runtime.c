#include "road_segment_runtime.h"

enum RoadLaneWidthReconciliationValue
{
  ROAD_LANE_WIDTH_CELL_MASK = TRACK_POSITION_UNIT - 1,
  ROAD_LANE_WIDTH_ROUND_BIAS = TRACK_POSITION_UNIT - 1
};

void
reconcile_connected_road_lane_runtime(RoadSegmentLaneRuntime *lane_runtime)
{
  RoadSegmentLaneRuntime *first_connected_lane;
  RoadSegmentLaneRuntime *second_connected_lane;
  int combined_left_width;
  int combined_right_width;
  int current_width;

  first_connected_lane = lane_runtime->previous_links[0];
  second_connected_lane = lane_runtime->previous_links[1];

  reconcile_linked_road_lane_surface_profile(
    lane_runtime, first_connected_lane);
  if(first_connected_lane != second_connected_lane)
    {
      reconcile_linked_road_lane_surface_profile(
        lane_runtime, second_connected_lane);
    }

  if(first_connected_lane->segment_left_bound >=
     lane_runtime->segment_left_bound)
    {
      lane_runtime->connected_left_bound =
        lane_runtime->segment_left_bound;
    }
  else
    {
      lane_runtime->connected_left_bound =
        first_connected_lane->segment_left_bound;
    }

  if(second_connected_lane->segment_right_bound <=
     lane_runtime->segment_right_bound)
    {
      lane_runtime->connected_right_bound =
        lane_runtime->segment_right_bound;
    }
  else
    {
      lane_runtime->connected_right_bound =
        second_connected_lane->segment_right_bound;
    }

  combined_left_width = first_connected_lane->left_width;
  combined_right_width = first_connected_lane->right_width;
  if(first_connected_lane != second_connected_lane)
    {
      combined_left_width += second_connected_lane->left_width;
      combined_right_width += second_connected_lane->right_width;
    }

  current_width = lane_runtime->left_width;
  if(combined_left_width != current_width &&
     (current_width & ROAD_LANE_WIDTH_CELL_MASK) == 0)
    {
      lane_runtime->left_width_cells = (unsigned char)(
        (combined_left_width + ROAD_LANE_WIDTH_ROUND_BIAS) >>
        TRACK_POSITION_FRACTION_BITS);
      if(combined_left_width > current_width)
        {
          lane_runtime->left_width = current_width + 1;
        }
    }

  current_width = lane_runtime->right_width;
  if(combined_right_width != current_width &&
     (current_width & ROAD_LANE_WIDTH_CELL_MASK) == 0)
    {
      lane_runtime->right_width_cells = (unsigned char)(
        (combined_right_width + ROAD_LANE_WIDTH_ROUND_BIAS) >>
        TRACK_POSITION_FRACTION_BITS);
      if(combined_right_width > current_width)
        {
          lane_runtime->right_width = current_width + 1;
        }
    }
}

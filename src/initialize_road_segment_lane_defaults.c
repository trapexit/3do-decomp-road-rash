#include "bss_late_data.h"

enum RoadSegmentLaneDefaultConstants
{
  ROAD_LANE_DEFAULT_TOPOLOGY_FLAGS = 7,
  ROAD_LANE_DEFAULT_WIDTH = 0x200,
  ROAD_LANE_WIDTH_SCALE = 250,
  ROAD_LANE_TRACK_ROUNDING_BIAS = 0xFF,
  ROAD_LANE_TRACK_FRACTION_BITS = 8,
  ROAD_LANE_DEFAULT_MARGIN = 0x96
};

int *
initialize_road_segment_lane_defaults(RoadSegmentLaneRuntime *lane_runtime,
                                      int                     unused_callback_index,
                                      int                     unused_callback_direction)
{
  int extent;

  (void)unused_callback_index;
  (void)unused_callback_direction;

  lane_runtime->topology_flags = ROAD_LANE_DEFAULT_TOPOLOGY_FLAGS;
  lane_runtime->surface_flags = 0;
  lane_runtime->surface_resource_ids[0] = 0;
  lane_runtime->surface_resource_ids[1] = 0;

  lane_runtime->right_width = ROAD_LANE_DEFAULT_WIDTH;
  lane_runtime->curvature_projection_step = 0;
  lane_runtime->left_width = ROAD_LANE_DEFAULT_WIDTH;
  lane_runtime->path_curvature_step = 0;
  lane_runtime->path_elevation_step = 0;
  lane_runtime->path_elevation = 0;
  lane_runtime->center_position = 0;

  extent = lane_runtime->left_width * ROAD_LANE_WIDTH_SCALE;
  if(extent < 0)
    {
      extent += ROAD_LANE_TRACK_ROUNDING_BIAS;
    }
  extent >>= ROAD_LANE_TRACK_FRACTION_BITS;
  lane_runtime->road_left = lane_runtime->center_position - extent;

  extent = lane_runtime->right_width * ROAD_LANE_WIDTH_SCALE;
  if(extent < 0)
    {
      extent += ROAD_LANE_TRACK_ROUNDING_BIAS;
    }
  extent >>= ROAD_LANE_TRACK_FRACTION_BITS;
  lane_runtime->road_right = lane_runtime->center_position + extent;
  lane_runtime->road_width =
    lane_runtime->road_right - lane_runtime->road_left;
  lane_runtime->shoulder_left =
    lane_runtime->road_left - ROAD_LANE_DEFAULT_MARGIN;
  lane_runtime->edge_depth_offsets[0] = 0;
  lane_runtime->shoulder_right =
    lane_runtime->road_right + ROAD_LANE_DEFAULT_MARGIN;
  lane_runtime->edge_depth_offsets[1] = 0;
  lane_runtime->segment_left_bound = lane_runtime->road_left;
  lane_runtime->segment_right_bound = lane_runtime->road_right;
  lane_runtime->edge_margins[ROAD_TRACK_FIRST_CHANNEL] =
    ROAD_LANE_DEFAULT_MARGIN;
  lane_runtime->edge_margins[ROAD_TRACK_SECOND_CHANNEL] =
    ROAD_LANE_DEFAULT_MARGIN;
  return &lane_runtime->edge_margins[ROAD_TRACK_SECOND_CHANNEL];
}

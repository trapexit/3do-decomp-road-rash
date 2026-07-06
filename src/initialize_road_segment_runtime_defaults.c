#include "bss_late_data.h"

enum RoadSegmentDefaultIndices
{
  ROAD_SEGMENT_PRIMARY_LANE = 0,
  ROAD_SEGMENT_SECONDARY_LANE = 1,
  ROAD_SEGMENT_SELECTED_LANE_TWO = 2,
  ROAD_SEGMENT_SELECTED_LANE_THREE = 3,
  ROAD_SEGMENT_DEFAULT_LANE_COUNT = 1,
  ROAD_SEGMENT_DEFAULT_TRANSITION_TYPE = -1,
  ROAD_LANE_DEFAULT_CALLBACK_INDEX = -1,
  ROAD_LANE_DEFAULT_CALLBACK_DIRECTION = 1
};

int *
initialize_road_segment_runtime_defaults(RoadSegmentRuntime *segment)
{
  RoadSegmentLaneRuntime *primary_lane;

  segment->track_position = 0;
  primary_lane = &segment->lanes[ROAD_SEGMENT_PRIMARY_LANE];
  segment->selected_lanes[ROAD_SEGMENT_PRIMARY_LANE] = primary_lane;
  segment->selected_lanes[ROAD_SEGMENT_SECONDARY_LANE] = primary_lane;
  segment->selected_lanes[ROAD_SEGMENT_SELECTED_LANE_TWO] = primary_lane;
  segment->selected_lanes[ROAD_SEGMENT_SELECTED_LANE_THREE] = primary_lane;
  segment->lanes[ROAD_SEGMENT_PRIMARY_LANE].resource_handle = segment;
  segment->lanes[ROAD_SEGMENT_SECONDARY_LANE].resource_handle = segment;
  segment->lane_count = ROAD_SEGMENT_DEFAULT_LANE_COUNT;
  segment->left_step = 0;
  segment->right_step = 0;
  segment->transition_type = ROAD_SEGMENT_DEFAULT_TRANSITION_TYPE;

  initialize_road_segment_lane_defaults(
    primary_lane, ROAD_LANE_DEFAULT_CALLBACK_INDEX,
    ROAD_LANE_DEFAULT_CALLBACK_DIRECTION);
  return initialize_road_segment_lane_defaults(
    &segment->lanes[ROAD_SEGMENT_SECONDARY_LANE],
    ROAD_LANE_DEFAULT_CALLBACK_INDEX,
    ROAD_LANE_DEFAULT_CALLBACK_DIRECTION);
}

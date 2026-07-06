#ifndef ROAD_SEGMENT_RUNTIME_H
#define ROAD_SEGMENT_RUNTIME_H

#include "bss_late_data.h"

enum RoadSegmentRuntimeLinkValue
{
  ROAD_SEGMENT_LINK_VARIANT_COUNT = 4,
  ROAD_SEGMENT_MAIN_FORK_LINK = 2,
  ROAD_SEGMENT_ALTERNATE_FORK_LINK = 3
};

enum RoadLaneTopologyFlag
{
  ROAD_LANE_TOPOLOGY_PRIMARY = 1,
  ROAD_LANE_TOPOLOGY_SECONDARY = 2,
  ROAD_LANE_TOPOLOGY_MAIN_FORK = 4,
  ROAD_LANE_TOPOLOGY_ALTERNATE_FORK = 8,
  ROAD_LANE_TOPOLOGY_SINGLE =
    ROAD_LANE_TOPOLOGY_PRIMARY |
    ROAD_LANE_TOPOLOGY_SECONDARY |
    ROAD_LANE_TOPOLOGY_MAIN_FORK
};

void
begin_road_segment_runtime_update(void);
void
select_road_track_channel_override(int channel_index);
void
reconcile_connected_road_lane_runtime(RoadSegmentLaneRuntime *lane_runtime);
void
populate_road_segment_lane_runtime(RoadSegmentLaneRuntime *lane_runtime,
                                   RoadClipPlaybackState  *playback,
                                   unsigned int            direction);

int
calculate_road_profile_intersection(const RoadLanePoint *left_start,
                                    const RoadLanePoint *left_end,
                                    const RoadLanePoint *right_start,
                                    const RoadLanePoint *right_end,
                                    RoadLanePoint       *intersection);
int
interpolate_road_profile_height(const RoadLanePoint *start,
                                const RoadLanePoint *end,
                                int                  lateral_position);
void
reconcile_road_lane_transition_profiles(RoadSegmentRuntime *segment);

void
populate_and_link_road_segment_runtime(int                      track_position,
                                       RoadTrackTraversalState *traversal);

RoadSegmentLaneRuntime *
move_road_lane_by_offset(RoadSegmentLaneRuntime *lane,
                         int                     position_offset);
RoadSegmentLaneRuntime *
find_road_lane_at_track_position(int                     track_position,
                                 RoadSegmentLaneRuntime *start_lane);

#endif

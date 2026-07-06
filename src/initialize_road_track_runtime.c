#include "bss_late_data.h"
#include "road_lane_schedule_runtime.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"
#include "track_segment_definition.h"

enum RoadTrackRuntimeInitializationValue
{
  ROAD_SEGMENT_POOL_ITEM_BYTES = 0xEC,
  ROAD_SEGMENT_POOL_ITEM_COUNT = 0x14,
  ROAD_TRACK_INITIAL_BACKWARD_SPAN = 0x3000,
  ROAD_TRACK_RUNTIME_SPAN = 0x8000,
  ROAD_FORWARD_SCHEDULE_WARMUP_COUNT = 0x5B,
  ROAD_REVERSE_SCHEDULE_WARMUP_COUNT = 0x0E,
  ROAD_SEGMENT_INDEX_MASK = ROAD_SEGMENT_COUNT - 1
};

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);
int
calculate_track_finish_distances(RoadTrackGraphNode *node,
                                 int                 distance_from_finish);
void
assign_track_graph_branch_parents(RoadTrackGraphNode *node,
                                  RoadTrackGraphNode *parent,
                                  int                 secondary_branch);
int
initialize_track_allocation_pools(void);
int *
initialize_road_segment_runtime_defaults(RoadSegmentRuntime *segment);

void
initialize_road_track_runtime(RoadTrackGraphNode *track_root,
                              int                 initial_track_position)
{
  RoadSegmentRuntime **segment_index;
  RoadSegmentRuntime *segment;
  RoadSegmentRuntime *next_segment;
  RoadSegmentRuntime *previous_segment;
  RoadSegmentLaneRuntime *lane;
  RoadTrackGraphLeafScheduleView *schedule_view;
  int index;
  int link_index;
  int first_track_position;

  track_runtime_globals.initialized = 0;
  track_runtime_globals.track_root = track_root;
  track_runtime_globals.finish_threshold = 0;
  track_runtime_globals.road_segment_pool = create_intrusive_object_pool(
    ROAD_SEGMENT_POOL_ITEM_BYTES, ROAD_SEGMENT_POOL_ITEM_COUNT, 0);
  track_runtime_globals.family_schedule_events_enabled = 0;
  if(track_root == 0 || track_runtime_globals.road_segment_pool == 0)
    {
      return;
    }

  if(prepare_track_resource_graph(track_root) == 0)
    {
      return;
    }
  track_runtime_globals.family_schedule_events_enabled = 1;
  calculate_track_finish_distances(track_root, -1);
  assign_track_graph_branch_parents(track_root, 0, 0);
  track_runtime_globals.async_family_resource_loads_enabled = 0;
  if(initialize_track_allocation_pools() == 0)
    {
      return;
    }

  create_road_track_traversal(&gRoadTrackForwardTraversal, track_root,
                              TRACK_TRAVERSAL_FORWARD);
  create_road_track_traversal(&gRoadTrackReverseTraversal, track_root,
                              TRACK_TRAVERSAL_REVERSE);

  track_runtime_globals.current_track_position = initial_track_position;
  first_track_position = initial_track_position -
                         ROAD_TRACK_INITIAL_BACKWARD_SPAN;
  track_runtime_globals.left_bound = first_track_position;
  track_runtime_globals.right_bound = first_track_position +
                                      ROAD_TRACK_RUNTIME_SPAN;
  seek_road_track_traversal(&gRoadTrackForwardTraversal,
                            first_track_position);
  seek_road_track_traversal(&gRoadTrackReverseTraversal,
                            first_track_position);

  segment_index = gRoadSegmentEffectWorkspace.segment_index.by_track_index;
  for(index = 0; index < ROAD_SEGMENT_COUNT; index++)
    {
      segment_index[index] = &gRoadSegments[index];
    }

  for(index = 0; index < ROAD_SEGMENT_COUNT; index++)
    {
      segment = segment_index[index];
      next_segment = segment_index[(index + 1) & ROAD_SEGMENT_INDEX_MASK];
      previous_segment =
        segment_index[(index - 1) & ROAD_SEGMENT_INDEX_MASK];
      segment->next = next_segment;
      segment->previous = previous_segment;
      for(link_index = 0;
          link_index < ROAD_SEGMENT_LINK_VARIANT_COUNT;
          link_index++)
        {
          segment->lanes[0].previous_links[link_index] =
            &next_segment->lanes[0];
          segment->lanes[1].previous_links[link_index] =
            &next_segment->lanes[1];
          segment->lanes[0].next_links[link_index] =
            &previous_segment->lanes[0];
          segment->lanes[1].next_links[link_index] =
            &previous_segment->lanes[1];
        }
    }

  index = (first_track_position >> TRACK_POSITION_FRACTION_BITS) &
          ROAD_SEGMENT_INDEX_MASK;
  segment = segment_index[index];
  for(index = 0; index < ROAD_SEGMENT_COUNT; index++)
    {
      initialize_road_segment_runtime_defaults(segment);
      segment->track_position = first_track_position +
                                (index << TRACK_POSITION_FRACTION_BITS);
      segment = segment->next;
    }

  for(index = 0; index < ROAD_SEGMENT_COUNT; index++)
    {
      populate_and_link_road_segment_runtime(
        first_track_position +
        (index << TRACK_POSITION_FRACTION_BITS),
        &gRoadTrackForwardTraversal);
      advance_road_track_traversal(&gRoadTrackForwardTraversal,
                                   TRACK_POSITION_UNIT);
    }

  index = (first_track_position >> TRACK_POSITION_FRACTION_BITS) &
          ROAD_SEGMENT_INDEX_MASK;
  lane = segment_index[index]->selected_lanes[ROAD_TRACK_FIRST_CHANNEL];
  create_road_lane_schedule_node(lane, first_track_position,
                                 TRACK_TRAVERSAL_FORWARD);
  create_road_lane_schedule_node(lane, first_track_position,
                                 TRACK_TRAVERSAL_REVERSE);
  schedule_view = (RoadTrackGraphLeafScheduleView *)lane->clip_node;
  schedule_view->bucket_lanes[TRACK_TRAVERSAL_REVERSE] = lane;
  schedule_view->bucket_lanes[TRACK_TRAVERSAL_FORWARD] = lane;

  for(index = 0; index < ROAD_FORWARD_SCHEDULE_WARMUP_COUNT; index++)
    {
      advance_road_lane_schedule_bucket(TRACK_TRAVERSAL_FORWARD,
                                        TRACK_POSITION_UNIT);
    }
  for(index = 0; index < ROAD_REVERSE_SCHEDULE_WARMUP_COUNT; index++)
    {
      advance_road_lane_schedule_bucket(TRACK_TRAVERSAL_REVERSE,
                                        TRACK_POSITION_UNIT);
    }

  track_runtime_globals.initialized = 1;
  track_runtime_globals.async_family_resource_loads_enabled = 1;
}

#ifndef ROAD_LANE_SCHEDULE_RUNTIME_H
#define ROAD_LANE_SCHEDULE_RUNTIME_H

#include "intrusive_list.h"
#include "road_segment_runtime.h"

#define ROAD_LANE_FIELD_OFFSET(type, member) \
        ((unsigned int)&(((type *)0)->member))

enum RoadLaneScheduleValue
{
  ROAD_LANE_SCHEDULE_BUCKET_COUNT = 2,
  ROAD_LANE_SCHEDULE_NODE_CAPACITY = 4,
  ROAD_LANE_SCHEDULE_FRACTION_BITS = 8,
  ROAD_LANE_SCHEDULE_FRACTION_MASK = 0xFF,
  ROAD_TRACK_GRAPH_LEAF_SCHEDULE_OFFSET = 0x18
};

typedef struct RoadLaneScheduleNode
{
  PooledListNode pooled;
  int bucket_index;
  int track_position;
  RoadSegmentLaneRuntime *lane;
} RoadLaneScheduleNode;

/* Leaf graph nodes reuse their junction-only tail as two runtime lane slots. */
typedef struct RoadTrackGraphLeafScheduleView
{
  unsigned char graph_fields[ROAD_TRACK_GRAPH_LEAF_SCHEDULE_OFFSET];
  RoadSegmentLaneRuntime
  *bucket_lanes[ROAD_LANE_SCHEDULE_BUCKET_COUNT];
} RoadTrackGraphLeafScheduleView;

typedef char RoadLaneScheduleNodeSizeCheck[
    sizeof(RoadLaneScheduleNode) == 0x18 ? 1 : -1];
typedef char RoadLaneScheduleBucketOffsetCheck[
    ROAD_LANE_FIELD_OFFSET(RoadLaneScheduleNode, bucket_index) == 0x0C
        ? 1 : -1];
typedef char RoadLaneSchedulePositionOffsetCheck[
    ROAD_LANE_FIELD_OFFSET(RoadLaneScheduleNode, track_position) == 0x10
        ? 1 : -1];
typedef char RoadLaneScheduleLaneOffsetCheck[
    ROAD_LANE_FIELD_OFFSET(RoadLaneScheduleNode, lane) == 0x14
        ? 1 : -1];
typedef char RoadTrackGraphLeafScheduleSizeCheck[
    sizeof(RoadTrackGraphLeafScheduleView) == 0x20 ? 1 : -1];
typedef char RoadTrackGraphLeafScheduleSlotsOffsetCheck[
    ROAD_LANE_FIELD_OFFSET(RoadTrackGraphLeafScheduleView, bucket_lanes) ==
    ROAD_TRACK_GRAPH_LEAF_SCHEDULE_OFFSET
        ? 1 : -1];

#undef ROAD_LANE_FIELD_OFFSET

RoadLaneScheduleNode *
create_road_lane_schedule_node(RoadSegmentLaneRuntime *lane,
                               int                     track_position,
                               int                     bucket_index);
int
release_road_lane_schedule_node(RoadLaneScheduleNode *schedule);
int
advance_road_lane_schedule_node(RoadLaneScheduleNode *schedule,
                                int                   delta);
IntrusiveListNode *
advance_road_lane_schedule_bucket(int bucket_index,
                                  int delta);

#endif

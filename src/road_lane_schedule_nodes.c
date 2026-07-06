#include "road_lane_schedule_runtime.h"
#include "rw_semantic_data.h"

IntrusiveListNode *
acquire_pooled_intrusive_node(IntrusiveListHeader *object_pool);

RoadLaneScheduleNode *
create_road_lane_schedule_node(RoadSegmentLaneRuntime *lane,
                               int                     track_position,
                               int                     bucket_index)
{
  IntrusiveListHeader *pool;
  IntrusiveListHeader *bucket;
  RoadLaneScheduleNode *schedule;
  int value;

  pool = (IntrusiveListHeader *)track_runtime_globals.allocation_pool;
  if(pool == 0)
    {
      return 0;
    }

  schedule = (RoadLaneScheduleNode *)
             acquire_pooled_intrusive_node(pool);
  if(schedule == 0)
    {
      return 0;
    }

  bucket = (IntrusiveListHeader *)
           track_runtime_globals.bucket_handles[bucket_index];
  if(bucket == 0)
    {
      return_pooled_node_to_owner(&schedule->pooled);
      return 0;
    }

  prepend_intrusive_list_node(bucket, &schedule->pooled.links);
  schedule->bucket_index = bucket_index;
  schedule->lane = lane;

  value = lane->clip_position;
  value = ((value ^ track_position) &
           ~ROAD_LANE_SCHEDULE_FRACTION_MASK) ^ track_position;
  schedule->track_position = value;
  return schedule;
}


int
release_road_lane_schedule_node(RoadLaneScheduleNode *schedule)
{
  return return_pooled_node_to_owner(&schedule->pooled);
}

#include "road_lane_schedule_runtime.h"
#include "rw_semantic_data.h"

IntrusiveListNode *
advance_road_lane_schedule_bucket(int bucket_index,
                                  int delta)
{
  IntrusiveListHeader *bucket;
  IntrusiveListNode *node;
  IntrusiveListNode *next;

  bucket = (IntrusiveListHeader *)
           track_runtime_globals.bucket_handles[bucket_index];
  if(bucket == 0)
    {
      return 0;
    }

  node = bucket->head_sentinel->next;
  if(node->next == 0)
    {
      return node;
    }

  do
    {
      next = node->next;
      advance_road_lane_schedule_node(
        (RoadLaneScheduleNode *)node, delta);
      node = next;
    } while(node->next != 0);

  return node;
}

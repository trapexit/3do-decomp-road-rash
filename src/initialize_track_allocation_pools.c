/* Reconstructed from the original function at 0x0001EDC4. */



#include "intrusive_list.h"
#include "road_lane_schedule_runtime.h"
#include "rw_semantic_data.h"

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);

int
initialize_track_allocation_pools(void)
{
  int result;

  result = (int)create_intrusive_object_pool(
    sizeof(RoadLaneScheduleNode), ROAD_LANE_SCHEDULE_NODE_CAPACITY, 0);
  track_runtime_globals.allocation_pool = (void *)result;
  if(result == 0)
    {
      return 0;
    }

  result = (int)initialize_intrusive_list_storage(0);
  track_runtime_globals.bucket_handles[0] = result;
  if(result == 0)
    {
      return 0;
    }

  result = (int)initialize_intrusive_list_storage(0);
  track_runtime_globals.bucket_handles[1] = result;

  return result;
}

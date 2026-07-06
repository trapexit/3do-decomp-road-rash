#include "intrusive_list.h"
#include "rw_semantic_data.h"
#include "track_segment_definition.h"

void
destroy_track_allocation_pools(void);

int
destroy_road_track_runtime(void)
{
  int result;
  int transition_result;

  track_runtime_globals.initialized = 0;
  track_runtime_globals.async_family_resource_loads_enabled = 0;
  track_runtime_globals.family_schedule_events_enabled = 0;
  result = destroy_intrusive_object_pool(
    (IntrusiveListStorage *)track_runtime_globals.road_segment_pool);
  if(result >= 0)
    {
      track_runtime_globals.road_segment_pool = 0;
    }
  destroy_track_allocation_pools();

  transition_result = release_road_track_graph_transitions(
    track_runtime_globals.track_root);
  if(transition_result >= 0)
    {
      track_runtime_globals.track_root = 0;
    }
  if(result < 0)
    {
      return result;
    }
  return transition_result;
}

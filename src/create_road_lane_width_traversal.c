#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x00021364. */

RoadRlanTraversalState *
create_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                                 const RoadRlanResource *resource)
{
  RoadRlanTraversalState *result;

  result = lane_state;
  if(result == 0)
    {
      result = MEM_NewPointer(sizeof(RoadRlanTraversalState),
                              MEMORY_REQUEST_ANY,
                              MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(result != 0)
    {
      initialize_road_lane_width_traversal(
        result, resource, TRACK_TRAVERSAL_FORWARD);
    }

  return result;
}

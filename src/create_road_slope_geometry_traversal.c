#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x000216A8. */

RoadRsldTraversalState *
create_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                     const RoadRsldResource *resource)
{
  RoadRsldTraversalState *result;

  result = slope_state;
  if(result == 0)
    {
      result = MEM_NewPointer(sizeof(RoadRsldTraversalState),
                              MEMORY_REQUEST_ANY,
                              MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(result != 0)
    {
      initialize_road_slope_geometry_traversal(
        result, resource, TRACK_TRAVERSAL_FORWARD);
    }

  return result;
}

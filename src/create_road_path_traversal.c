#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001FDA4. */

RoadPathTraversalState *
create_road_path_traversal(RoadPathTraversalState *path_state,
                           const RoadPathResource *resource)
{
  if(path_state == 0)
    {
      path_state = (RoadPathTraversalState *)MEM_NewPointer(
        sizeof(*path_state), MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(path_state != 0)
    {
      path_state->elevation = 0;
      initialize_road_path_traversal(
        path_state, resource, TRACK_TRAVERSAL_FORWARD);
    }

  return path_state;
}

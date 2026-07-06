#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0002005C. */

RoadObjectTraversalState *
create_road_object_traversal(RoadObjectTraversalState *object_state,
                             const RoadObjectResource *resource)
{
  if(object_state == 0)
    {
      object_state = (RoadObjectTraversalState *)MEM_NewPointer(
        sizeof(*object_state), MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(object_state != 0)
    {
      initialize_road_object_traversal(
        object_state, resource, TRACK_TRAVERSAL_FORWARD);
    }

  return object_state;
}

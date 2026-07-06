#include "bss_late_data.h"
#include "memory_manager_runtime.h"

/* Reconstructed from the original function at 0x00021070. */

RoadSectionTraversalState *
create_road_section_traversal(RoadSectionTraversalState *traversal,
                              RoadSectionResource       *resource)
{
  if(traversal == 0)
    {
      traversal = (RoadSectionTraversalState *)MEM_NewPointer(
        sizeof(*traversal), MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(traversal != 0)
    {
      initialize_road_section_traversal(
        traversal, resource, TRACK_TRAVERSAL_FORWARD);
    }

  return traversal;
}

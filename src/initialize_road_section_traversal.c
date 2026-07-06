#include "bss_late_data.h"

/* Reconstructed from the original function at 0x0002101C. */

void *
initialize_road_section_traversal(RoadSectionTraversalState *traversal,
                                  RoadSectionResource       *resource,
                                  int                        forward)
{
  int entry_index;

  traversal->resource = resource;
  if(resource == 0)
    {
      return traversal;
    }

  if((forward & TRACK_DIRECTION_BYTE_MASK) != 0)
    {
      traversal->position = 0;
      traversal->entry_index = 0;
    }
  else
    {
      traversal->position =
        resource->sample_count << TRACK_POSITION_FRACTION_BITS;
      traversal->entry_index = resource->entry_count - 1;
    }

  entry_index = traversal->entry_index;
  traversal->active_entry = &resource->entries[entry_index];
  return &traversal->active_entry;
}

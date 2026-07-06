#include "bss_late_data.h"

/* Reconstructed from the original function at 0x000210C0. */

void
advance_road_section_traversal(RoadSectionTraversalState *traversal,
                               int                        delta)
{
  RoadSectionResource *resource;
  RoadSectionEntry *entry;
  int new_position;
  int sample_position;
  int entry_count;
  int entry_index;

  new_position = traversal->position + delta;
  sample_position = new_position >> TRACK_POSITION_FRACTION_BITS;

  resource = traversal->resource;
  if(resource != 0)
    {
      entry_count = resource->entry_count;
      if(entry_count > 1)
        {
          if(delta > 0)
            {
              entry_index = traversal->entry_index;
              if(entry_index < entry_count - 1 &&
                 (traversal->active_entry + 1)->start_sample <=
                 sample_position)
                {
                  traversal->entry_index = entry_index + 1;
                  traversal->active_entry++;
                }
            }
          else if(delta < 0)
            {
              entry_index = traversal->entry_index;
              if(entry_index > 0 &&
                 sample_position <
                 traversal->active_entry->start_sample)
                {
                  traversal->entry_index = entry_index - 1;
                  traversal->active_entry--;
                }
            }
        }

      entry = traversal->active_entry;
      if(entry->start_sample <= sample_position &&
         sample_position <= entry->end_sample)
        {
          traversal->active_kind = (unsigned char)(entry->kind + 1);
        }
      else
        {
          traversal->active_kind = 0;
        }
    }

  traversal->position = new_position;
}

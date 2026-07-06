/* Reconstructed from the original function at 0x00020920. */

#include "track_segment_definition.h"

void
advance_track_hazard_traversal(TrackHazardTraversal *traversal,
                               int                   delta)
{
  TrackHazardTable *table;
  TrackHazardEntry *entry;
  int target;
  int index;
  int limit;
  int key;

  traversal->track_position += delta;
  table = traversal->table;
  if(table == 0)
    {
      return;
    }

  limit = get_track_hazard_entry_count(table);
  index = traversal->entry_index;
  if(index < 0 || index >= limit)
    {
      return;
    }

  target = traversal->track_position >> TRACK_POSITION_FRACTION_BITS;
  entry = &table->entries[index];

  if(delta > 0)
    {
      for(;;)
        {
          index = traversal->entry_index;
          if(index >= limit)
            {
              return;
            }
          key = (entry->control_word >> TRACK_HAZARD_FRAME_SHIFT) &
                TRACK_HAZARD_FRAME_MASK;
          if(key >= target)
            {
              return;
            }
          index += 1;
          entry++;
          traversal->entry_index = index;
        }
    }

  if(delta >= 0)
    {
      return;
    }

  for(;;)
    {
      index = traversal->entry_index;
      if(index == 0)
        {
          return;
        }
      key = (entry->control_word >> TRACK_HAZARD_FRAME_SHIFT) &
            TRACK_HAZARD_FRAME_MASK;
      if(key <= target)
        {
          return;
        }
      index -= 1;
      entry--;
      traversal->entry_index = index;
    }
}

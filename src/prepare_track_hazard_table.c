/* Reconstructed from the original function at 0x00020670. */

#include "track_segment_definition.h"

void
schedule_track_hazard_entries(TrackHazardTable *table);

void
prepare_track_hazard_table(TrackHazardTable *table)
{
  int i;
  int j;
  int count;
  TrackHazardEntry *left;
  TrackHazardEntry *right;
  TrackHazardEntry saved_left;

  if(table == 0)
    {
      return;
    }

  schedule_track_hazard_entries(table);

  count = get_track_hazard_entry_count(table);
  for(i = 0; i < count - 1; i++)
    {
      left = &table->entries[i];
      for(j = i + 1; j < count; j++)
        {
          right = &table->entries[j];
          if(((right->control_word >> TRACK_HAZARD_FRAME_SHIFT) &
              TRACK_HAZARD_FRAME_MASK) <
             ((left->control_word >> TRACK_HAZARD_FRAME_SHIFT) &
              TRACK_HAZARD_FRAME_MASK))
            {
              saved_left = *left;
              *left = *right;
              *right = saved_left;
            }
        }
    }
}

#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020E30 entry of the original function at 0x00020D64. */

void
seek_track_boundary_index(TrackBoundaryCursor *cursor,
                          int                  forward)
{
  const TrackBoundaryTable *table;
  const TrackBoundaryEntry *entry;
  int current_sample;
  int boundary_index;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  table = cursor->table;
  if(table == 0)
    {
      return;
    }

  boundary_index = cursor->boundary_index;
  current_sample = cursor->current_sample;
  entry = &table->entries[boundary_index];

  if(forward != TRACK_TRAVERSAL_REVERSE)
    {
      if(entry->sample_index > current_sample)
        {
          return;
        }
      do
        {
          boundary_index++;
          cursor->boundary_index = boundary_index;
          entry = &table->entries[boundary_index];
        } while(entry->sample_index <= current_sample);
      return;
    }

  if(entry->sample_index < current_sample)
    {
      return;
    }
  do
    {
      boundary_index--;
      cursor->boundary_index = boundary_index;
      entry = &table->entries[boundary_index];
    } while(entry->sample_index >= current_sample);
}

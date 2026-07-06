#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020EB4 entry of the original function at 0x00020D64. */

void
synchronize_track_boundary_cursor(TrackBoundaryCursor *cursor)
{
  int previous_sample;
  int current_sample;

  previous_sample = cursor->current_sample;
  cursor->previous_sample = previous_sample;
  current_sample =
    cursor->track_position >> TRACK_POSITION_FRACTION_BITS;
  cursor->current_sample = current_sample;

  if(current_sample > previous_sample)
    {
      seek_track_boundary_index(cursor, TRACK_TRAVERSAL_FORWARD);
    }
  else if(current_sample < previous_sample)
    {
      seek_track_boundary_index(cursor, TRACK_TRAVERSAL_REVERSE);
    }
}

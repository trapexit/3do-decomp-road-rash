#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020EE4 entry of the original function at 0x00020D64. */

void
advance_track_boundary_cursor(TrackBoundaryCursor *cursor,
                              int                  delta)
{
  cursor->track_position += delta;
}

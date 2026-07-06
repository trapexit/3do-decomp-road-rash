#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020F88 entry of the original function at 0x00020D64. */

void
advance_signed_track_sample_cursor(SignedTrackSampleCursor *cursor,
                                   int                      delta)
{
  cursor->track_position += delta;
}

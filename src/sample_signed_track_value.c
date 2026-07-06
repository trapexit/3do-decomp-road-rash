#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020F6C entry of the original function at 0x00020D64. */

int
sample_signed_track_value(const SignedTrackSampleCursor *cursor)
{
  return cursor->series->samples[
    cursor->track_position >> TRACK_POSITION_FRACTION_BITS];
}

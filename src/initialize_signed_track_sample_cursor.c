#include "stddef.h"

#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020EF4 entry of the original function at 0x00020D64. */

typedef char SignedTrackSampleSeriesSizeIs8[
    (sizeof(SignedTrackSampleSeries) == 0x08) ? 1 : -1];
typedef char SignedTrackSampleCursorSizeIs8[
    (sizeof(SignedTrackSampleCursor) == 0x08) ? 1 : -1];
typedef char SignedTrackSamplesOffsetIs4[
    (offsetof(SignedTrackSampleSeries, samples) == 0x04) ? 1 : -1];

void
initialize_signed_track_sample_cursor(SignedTrackSampleCursor       *cursor,
                                      const SignedTrackSampleSeries *series,
                                      int                            forward)
{
  forward &= TRACK_DIRECTION_BYTE_MASK;
  cursor->series = series;
  if(series == 0)
    {
      return;
    }

  if(forward == TRACK_TRAVERSAL_REVERSE)
    {
      cursor->track_position =
        series->terminal_sample_index << TRACK_POSITION_FRACTION_BITS;
    }
  else
    {
      cursor->track_position = 0;
    }
}

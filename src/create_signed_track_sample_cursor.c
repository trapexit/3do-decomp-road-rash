#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020F1C entry of the original function at 0x00020D64. */

SignedTrackSampleCursor *
create_signed_track_sample_cursor(SignedTrackSampleCursor       *cursor,
                                  const SignedTrackSampleSeries *series)
{
  SignedTrackSampleCursor *result;

  result = cursor;
  if(result == 0)
    {
      result = MEM_NewPointer(sizeof(SignedTrackSampleCursor),
                              MEMORY_REQUEST_ANY,
                              MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(result != 0)
    {
      initialize_signed_track_sample_cursor(
        result, series, TRACK_TRAVERSAL_FORWARD);
    }

  return result;
}

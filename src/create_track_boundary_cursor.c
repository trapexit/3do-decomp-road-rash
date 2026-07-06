#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020DE0 entry of the original function at 0x00020D64. */

TrackBoundaryCursor *
create_track_boundary_cursor(TrackBoundaryCursor      *cursor,
                             const TrackBoundaryTable *table)
{
  TrackBoundaryCursor *result;

  result = cursor;
  if(result == 0)
    {
      result = MEM_NewPointer(sizeof(TrackBoundaryCursor),
                              MEMORY_REQUEST_ANY,
                              MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(result != 0)
    {
      initialize_track_boundary_cursor(
        result, table, TRACK_TRAVERSAL_FORWARD);
    }

  return result;
}

#include "stddef.h"

#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020DA8 entry of the original function at 0x00020D64. */

typedef char TrackBoundaryEntrySizeIs8[
    (sizeof(TrackBoundaryEntry) == 0x08) ? 1 : -1];
typedef char TrackBoundaryTableEntriesOffsetIs20[
    (offsetof(TrackBoundaryTable, entries) == 0x14) ? 1 : -1];
typedef char TrackBoundaryCursorSizeIs20[
    (sizeof(TrackBoundaryCursor) == 0x14) ? 1 : -1];

void
initialize_track_boundary_cursor(TrackBoundaryCursor      *cursor,
                                 const TrackBoundaryTable *table,
                                 int                       forward)
{
  int zero;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  cursor->table = table;
  if(table == 0)
    {
      return;
    }

  if(forward == TRACK_TRAVERSAL_REVERSE)
    {
      cursor->boundary_index = table->entry_count - 1;
      return;
    }

  zero = 0;
  cursor->boundary_index = zero;
  cursor->track_position = zero;
  cursor->current_sample = zero;
}

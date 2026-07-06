/* Reconstructed from the original function at 0x0002069C. */

#include "track_segment_definition.h"

int *
initialize_track_hazard_traversal(TrackHazardTraversal *traversal,
                                  TrackHazardTable     *table,
                                  int                   sample_count,
                                  int                   forward)
{
  int entry_count;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  traversal->table = table;
  if(table == 0)
    {
      return (int *)traversal;
    }

  if(forward != 0)
    {
      traversal->entry_index = 0;
      traversal->track_position = 0;
      return &traversal->track_position;
    }

  entry_count = get_track_hazard_entry_count(table);
  traversal->entry_index = entry_count - 1;
  traversal->track_position =
    (sample_count - 1) << TRACK_POSITION_FRACTION_BITS;
  return &traversal->track_position;
}

/* Reconstructed from the original function at 0x000206DC. */

#include "track_segment_definition.h"

#include "memory_manager_runtime.h"
int *
initialize_track_hazard_traversal(TrackHazardTraversal *traversal,
                                  TrackHazardTable     *table,
                                  int                   sample_count,
                                  int                   forward);

TrackHazardTraversal *
create_track_hazard_traversal(TrackHazardTraversal *traversal,
                              TrackHazardTable     *table)
{
  if(traversal == 0)
    {
      traversal = MEM_NewPointer(sizeof(TrackHazardTraversal),
                                 MEMORY_REQUEST_ANY,
                                 MEM_POINTER_OPTIONS_DEFAULT);
    }
  if(traversal != 0)
    {
      initialize_track_hazard_traversal(traversal, table, 0, 1);
    }
  return traversal;
}

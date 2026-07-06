#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x000209C8. */

void
initialize_road_family_schedule_traversal(RoadFamilyScheduleTraversalState *schedule_state,
                                          RoadFamilyScheduleResource       *resource,
                                          int                               track_position)
{
  schedule_state->resource = resource;
  if(resource == 0)
    {
      return;
    }

  if(track_position != 0)
    {
      schedule_state->entry_index = resource->entry_count - 1;
    }
  else
    {
      schedule_state->entry_index = 0;
    }
  schedule_state->track_position = track_position;
  schedule_state->sample_index =
    track_position >> TRACK_POSITION_FRACTION_BITS;
}

#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020D64 entry of the original function at 0x00020D64. */

void
advance_road_family_schedule_traversal(RoadFamilyScheduleTraversalState *schedule_state,
                                       int                               delta)
{
  int previous_sample;
  int new_position;
  int new_sample;

  new_position = schedule_state->track_position + delta;
  previous_sample = schedule_state->sample_index;
  schedule_state->track_position = new_position;
  new_sample = new_position >> TRACK_POSITION_FRACTION_BITS;
  schedule_state->sample_index = new_sample;

  if(new_sample == previous_sample || schedule_state->resource == 0)
    {
      return;
    }

  if(delta > 0)
    {
      process_road_family_schedule_events(
        schedule_state, TRACK_TRAVERSAL_FORWARD);
    }
  else if(delta < 0)
    {
      process_road_family_schedule_events(
        schedule_state, TRACK_TRAVERSAL_REVERSE);
    }
}

#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001FFA0. */

void
initialize_road_object_traversal(RoadObjectTraversalState *object_state,
                                 const RoadObjectResource *resource,
                                 int                       forward)
{
  int active_span;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  object_state->resource = resource;
  if(resource == 0)
    {
      return;
    }

  if(forward != 0)
    {
      object_state->entry_index = 0;
      object_state->track_position = 0;
      decode_road_object_entry(object_state,
                               object_state->entry_index);
      object_state->active_start_sample = 0;
      object_state->current_sample = 0;
      active_span = object_state->row_count_minus_one *
                    (object_state->row_spacing_minus_one + 1);
      object_state->active_end_sample = active_span;
      return;
    }

  object_state->entry_index = resource->entry_count - 1;
  object_state->track_position =
    (resource->sample_count - 1) * TRACK_POSITION_UNIT;
  decode_road_object_entry(object_state,
                           object_state->entry_index);
  object_state->current_sample =
    object_state->track_position >> TRACK_POSITION_FRACTION_BITS;
  object_state->active_end_sample = object_state->current_sample;
  active_span = object_state->row_count_minus_one *
                (object_state->row_spacing_minus_one + 1);
  object_state->active_start_sample =
    object_state->current_sample - active_span;
}

#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0002033C. */

void
advance_road_object_traversal(RoadObjectTraversalState *object_state,
                              int                       delta)
{
  int previous_sample;
  int current_sample;
  int active_span;

  if(object_state->resource == 0)
    {
      return;
    }

  previous_sample = object_state->track_position >>
                    TRACK_POSITION_FRACTION_BITS;
  object_state->track_position += delta;
  current_sample = object_state->track_position >>
                   TRACK_POSITION_FRACTION_BITS;
  if(current_sample == previous_sample)
    {
      return;
    }

  object_state->current_sample = current_sample;
  if(delta > 0)
    {
      if((unsigned int)current_sample <=
         (unsigned int)object_state->active_end_sample)
        {
          return;
        }
      if(object_state->entry_index >=
         object_state->resource->entry_count - 1)
        {
          object_state->resource_selector =
            ROAD_OBJECT_DISABLED_SELECTOR;
          return;
        }

      object_state->entry_index++;
      decode_road_object_entry(object_state,
                               object_state->entry_index);
      object_state->active_start_sample = current_sample;
      active_span = object_state->row_count_minus_one *
                    (object_state->row_spacing_minus_one + 1);
      object_state->active_end_sample =
        current_sample + active_span;
      return;
    }

  if((unsigned int)current_sample >=
     (unsigned int)object_state->active_start_sample)
    {
      return;
    }
  if(object_state->entry_index <= 0)
    {
      object_state->resource_selector =
        ROAD_OBJECT_DISABLED_SELECTOR;
      return;
    }

  object_state->entry_index--;
  decode_road_object_entry(object_state,
                           object_state->entry_index);
  object_state->active_end_sample = current_sample;
  active_span = object_state->row_count_minus_one *
                (object_state->row_spacing_minus_one + 1);
  object_state->active_start_sample =
    current_sample - active_span;
}

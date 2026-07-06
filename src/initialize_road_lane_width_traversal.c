#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x000212AC. */

void *
initialize_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                                     const RoadRlanResource *resource,
                                     int                     forward)
{
  const RoadRlanEntry *entry;
  const RoadRlanEntry *last_entry;
  int entry_index;
  int terminal_sample;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  lane_state->resource = resource;
  if(resource == 0)
    {
      return lane_state;
    }

  if(forward != TRACK_TRAVERSAL_REVERSE)
    {
      entry = &resource->entries[0];
      lane_state->current_entry = entry;
      lane_state->entry_index = 0;
      lane_state->left_width_accumulator =
        entry->start_left_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      lane_state->track_position = 0;
      lane_state->right_width_accumulator =
        entry->start_right_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;

      last_entry =
        &resource->entries[resource->entry_count - 1];
      terminal_sample = last_entry->end_sample;
    }
  else
    {
      entry_index = resource->entry_count - 1;
      lane_state->entry_index = entry_index;
      entry = &resource->entries[entry_index];
      lane_state->current_entry = entry;
      lane_state->left_width_accumulator =
        entry->end_left_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      lane_state->right_width_accumulator =
        entry->end_right_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;

      terminal_sample = entry->end_sample;
      lane_state->track_position =
        terminal_sample << TRACK_POSITION_FRACTION_BITS;
    }

  lane_state->end_position =
    terminal_sample << TRACK_POSITION_FRACTION_BITS;
  /* The original final writeback leaves this field address in r0. */
  return &lane_state->end_position;
}

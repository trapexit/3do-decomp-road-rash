#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x000215C0. */

void *
initialize_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                         const RoadRsldResource *resource,
                                         int                     forward)
{
  const RoadRsldEntry *entry;
  const RoadRsldEntry *last_entry;
  int entry_index;
  int terminal_sample;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  slope_state->resource = resource;
  if(resource == 0)
    {
      return slope_state;
    }

  if(forward != TRACK_TRAVERSAL_REVERSE)
    {
      entry = &resource->entries[0];
      slope_state->current_entry = entry;
      slope_state->entry_index = 0;
      slope_state->left_margin_accumulator =
        entry->start_left_margin <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->right_margin_accumulator =
        entry->start_right_margin <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->left_depth_accumulator =
        entry->start_left_depth <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->track_position = 0;
      slope_state->right_depth_accumulator =
        entry->start_right_depth <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;

      last_entry =
        &resource->entries[resource->entry_count - 1];
      terminal_sample = last_entry->end_sample;
    }
  else
    {
      entry_index = resource->entry_count - 1;
      slope_state->entry_index = entry_index;
      entry = &resource->entries[entry_index];
      slope_state->current_entry = entry;
      slope_state->left_margin_accumulator =
        entry->end_left_margin <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->right_margin_accumulator =
        entry->end_right_margin <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->left_depth_accumulator =
        entry->end_left_depth <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->right_depth_accumulator =
        entry->end_right_depth <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;

      terminal_sample = entry->end_sample;
      slope_state->track_position =
        terminal_sample << TRACK_POSITION_FRACTION_BITS;
    }

  slope_state->end_position =
    terminal_sample << TRACK_POSITION_FRACTION_BITS;
  /* The original final writeback leaves this field address in r0. */
  return &slope_state->end_position;
}

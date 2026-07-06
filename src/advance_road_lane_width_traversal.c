#include "track_traversal_runtime.h"

/* Reconstructed from the 0x000213E8 entry of the original function at 0x000213E8. */

void
advance_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                                  int                     delta)
{
  const RoadRlanEntry *entry;
  int old_position;
  int old_sample;
  int new_sample;

  old_position = lane_state->track_position;
  old_sample = old_position >> TRACK_POSITION_FRACTION_BITS;
  lane_state->track_position = old_position + delta;

  if(lane_state->resource == 0)
    {
      return;
    }

  new_sample =
    lane_state->track_position >> TRACK_POSITION_FRACTION_BITS;
  if(new_sample == old_sample)
    {
      return;
    }

  entry = lane_state->current_entry;
  if(delta > 0)
    {
      if(new_sample > entry->end_sample)
        {
          if(new_sample == (entry + 1)->start_sample)
            {
              lane_state->current_entry = entry + 1;
              lane_state->entry_index++;
            }
          return;
        }

      if(new_sample < entry->end_sample)
        {
          lane_state->left_width_accumulator +=
            entry->left_width_step;
          lane_state->right_width_accumulator +=
            entry->right_width_step;
          return;
        }

      lane_state->left_width_accumulator =
        entry->end_left_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      lane_state->right_width_accumulator =
        entry->end_right_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      return;
    }

  if(delta >= 0)
    {
      return;
    }
  if(new_sample >= entry->end_sample)
    {
      return;
    }
  if(new_sample > entry->start_sample)
    {
      lane_state->left_width_accumulator -= entry->left_width_step;
      lane_state->right_width_accumulator -= entry->right_width_step;
      return;
    }
  if(new_sample == entry->start_sample)
    {
      lane_state->left_width_accumulator =
        entry->start_left_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      lane_state->right_width_accumulator =
        entry->start_right_width <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      return;
    }

  lane_state->current_entry = entry - 1;
  lane_state->entry_index--;
}

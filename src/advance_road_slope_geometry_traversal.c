#include "track_traversal_runtime.h"

/* Reconstructed from the 0x0002176C entry of the original function at 0x0002176C. */

void
advance_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                      int                     delta)
{
  const RoadRsldEntry *entry;
  int old_position;
  int old_sample;
  int new_sample;

  old_position = slope_state->track_position;
  old_sample = old_position >> TRACK_POSITION_FRACTION_BITS;
  slope_state->track_position = old_position + delta;

  if(slope_state->resource == 0)
    {
      return;
    }

  new_sample =
    slope_state->track_position >> TRACK_POSITION_FRACTION_BITS;
  if(new_sample == old_sample)
    {
      return;
    }

  entry = slope_state->current_entry;
  if(delta > 0)
    {
      if(new_sample > entry->end_sample)
        {
          if(new_sample == (entry + 1)->start_sample)
            {
              slope_state->current_entry = entry + 1;
              slope_state->entry_index++;
            }
          return;
        }

      if(new_sample >= entry->end_sample)
        {
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
          return;
        }

      slope_state->left_margin_accumulator += entry->left_margin_step;
      slope_state->right_margin_accumulator += entry->right_margin_step;
      slope_state->left_depth_accumulator += entry->left_depth_step;
      slope_state->right_depth_accumulator += entry->right_depth_step;
      return;
    }

  if(delta < 0)
    {
      if(new_sample >= entry->end_sample)
        {
          return;
        }
      if(new_sample > entry->start_sample)
        {
          slope_state->left_margin_accumulator -=
            entry->left_margin_step;
          slope_state->right_margin_accumulator -=
            entry->right_margin_step;
          slope_state->left_depth_accumulator -= entry->left_depth_step;
          slope_state->right_depth_accumulator -=
            entry->right_depth_step;
          return;
        }
      if(new_sample != entry->start_sample)
        {
          slope_state->current_entry = entry - 1;
          slope_state->entry_index--;
          return;
        }

      slope_state->left_margin_accumulator =
        entry->start_left_margin <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->right_margin_accumulator =
        entry->start_right_margin <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->left_depth_accumulator =
        entry->start_left_depth <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
      slope_state->right_depth_accumulator =
        entry->start_right_depth <<
          ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
    }
}

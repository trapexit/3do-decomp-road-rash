#include "bss_late_data.h"

/* Reconstructed from the original function at 0x000216F8. */

void
sample_road_slope_geometry(const RoadRsldTraversalState *slope_state,
                           int                          *out_margins,
                           int                          *out_depth_offsets,
                           unsigned char                *out_edge_resource_ids)
{
  int value;

  if(slope_state->resource == 0)
    {
      value = 0;
      out_margins[ROAD_TRACK_FIRST_CHANNEL] = value;
      out_margins[ROAD_TRACK_SECOND_CHANNEL] = value;
      out_depth_offsets[ROAD_TRACK_FIRST_CHANNEL] = value;
      out_depth_offsets[ROAD_TRACK_SECOND_CHANNEL] = value;
      out_edge_resource_ids[ROAD_TRACK_FIRST_CHANNEL] =
        (unsigned char)value;
      out_edge_resource_ids[ROAD_TRACK_SECOND_CHANNEL] =
        (unsigned char)value;
    }

  /* The original routine deliberately continues from its empty-chunk case. */

  out_margins[ROAD_TRACK_FIRST_CHANNEL] =
    slope_state->left_margin_accumulator >>
    ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
  out_margins[ROAD_TRACK_SECOND_CHANNEL] =
    slope_state->right_margin_accumulator >>
    ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
  out_depth_offsets[ROAD_TRACK_FIRST_CHANNEL] =
    slope_state->left_depth_accumulator >>
    ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
  out_depth_offsets[ROAD_TRACK_SECOND_CHANNEL] =
    slope_state->right_depth_accumulator >>
    ROAD_INTERPOLATED_VALUE_FRACTION_BITS;
  out_edge_resource_ids[ROAD_TRACK_FIRST_CHANNEL] =
    slope_state->current_entry
    ->edge_resource_ids[ROAD_TRACK_FIRST_CHANNEL];
  out_edge_resource_ids[ROAD_TRACK_SECOND_CHANNEL] =
    slope_state->current_entry
    ->edge_resource_ids[ROAD_TRACK_SECOND_CHANNEL];
}

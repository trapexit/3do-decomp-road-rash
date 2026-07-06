#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001FEBC. */

void
decode_road_object_entry(RoadObjectTraversalState *object_state,
                         int                       entry_index)
{
  unsigned int packed;
  unsigned int spacing_code;

  packed = object_state->resource->entries[entry_index];
  object_state->alternate_sides = (unsigned char)(
    (packed >> ROAD_OBJECT_ALTERNATE_SIDES_SHIFT) &
    ROAD_OBJECT_SINGLE_BIT_MASK);
  object_state->mirror_sprite = (unsigned char)(
    (packed >> ROAD_OBJECT_MIRROR_SPRITE_SHIFT) &
    ROAD_OBJECT_SINGLE_BIT_MASK);
  object_state->wide_spacing = (unsigned char)(
    (packed >> ROAD_OBJECT_WIDE_SPACING_SHIFT) &
    ROAD_OBJECT_SINGLE_BIT_MASK);
  object_state->base_left_side = (unsigned char)(
    (packed >> ROAD_OBJECT_BASE_LEFT_SIDE_SHIFT) &
    ROAD_OBJECT_SINGLE_BIT_MASK);
  object_state->resource_selector = (unsigned char)(
    (packed >> ROAD_OBJECT_RESOURCE_SELECTOR_SHIFT) &
    ROAD_OBJECT_RESOURCE_SELECTOR_MASK);
  object_state->randomize_lateral_spacing = (unsigned char)(
    (packed >> ROAD_OBJECT_RANDOM_LATERAL_SPACING_SHIFT) &
    ROAD_OBJECT_SINGLE_BIT_MASK);
  object_state->column_count_minus_one = (unsigned char)(
    (packed >> ROAD_OBJECT_COLUMN_COUNT_SHIFT) &
    ROAD_OBJECT_COLUMN_COUNT_MASK);
  object_state->scale_eighths = (unsigned char)(
    (packed >> ROAD_OBJECT_SCALE_SHIFT) &
    ROAD_OBJECT_NIBBLE_MASK);
  object_state->row_count_minus_one = (unsigned char)(
    (packed >> ROAD_OBJECT_ROW_COUNT_SHIFT) &
    ROAD_OBJECT_NIBBLE_MASK);

  if(object_state->row_count_minus_one != 0)
    {
      spacing_code = packed & ROAD_OBJECT_NIBBLE_MASK;
      object_state->row_spacing_minus_one = (unsigned char)(
        (packed >> ROAD_OBJECT_ROW_SPACING_SHIFT) &
        ROAD_OBJECT_NIBBLE_MASK);
      if(object_state->wide_spacing == 0)
        {
          object_state->lateral_spacing =
            (int)(spacing_code <<
                  ROAD_OBJECT_NORMAL_SPAN_SPACING_SHIFT);
        }
      else
        {
          object_state->lateral_spacing =
            ROAD_OBJECT_LARGE_SPACING_BASE +
            (int)(spacing_code <<
                  ROAD_OBJECT_LARGE_SPAN_SPACING_SHIFT);
        }
    }
  else
    {
      spacing_code =
        ((packed >> ROAD_OBJECT_COLUMN_COUNT_SHIFT) &
         ROAD_OBJECT_SPACING_FIELD_MASK) +
        (packed & ROAD_OBJECT_NIBBLE_MASK);
      object_state->row_spacing_minus_one = 0;
      if(object_state->wide_spacing == 0)
        {
          object_state->lateral_spacing =
            (int)(spacing_code <<
                  ROAD_OBJECT_NORMAL_POINT_SPACING_SHIFT);
        }
      else
        {
          object_state->lateral_spacing =
            ROAD_OBJECT_LARGE_SPACING_BASE +
            (int)(spacing_code <<
                  ROAD_OBJECT_LARGE_POINT_SPACING_SHIFT);
        }
    }

  object_state->entry_runtime_flags = 0;
}

#include "cans_animation_binding.h"
#include "division_helpers.h"

enum CansFrameRenderConstant
{
  CANS_RENDER_FIXED_SHIFT = 8,
  CANS_RENDER_FIXED_ROUNDING_BIAS = 0xFF,
  CANS_RENDER_ANGLE_WRAP_LIMIT = 0x7F0000,
  CANS_RENDER_ANGLE_RANGE = 0xFF0000,
  CANS_RENDER_ANGLE_CENTER = 0x800000,
  CANS_RENDER_FRAME_FRACTION_SHIFT = 16,
  CANS_RENDER_FRAME_SCALE_SHIFT = 12,
  CANS_RENDER_FRAME_PRODUCT_SHIFT = 4,
  CANS_RENDER_FRAME_ROUNDING = 0x8000,
  CANS_RENDER_SPECIAL_EXTENT_BASE = 0xFA,
  CANS_RENDER_EXTENT_TWO = 2,
  CANS_RENDER_EXTENT_FOUR = 4,
  CANS_RENDER_EXTENT_EIGHT = 8,
  CANS_RENDER_EXTENT_TWO_SHIFT = 7,
  CANS_RENDER_EXTENT_FOUR_SHIFT = 6,
  CANS_RENDER_EXTENT_EIGHT_SHIFT = 5,
  CANS_RENDER_COLUMN_EXTENT_SHIFT = 15,
  CANS_RENDER_COLUMN_SCALE_SHIFT = 16,
  CANS_RENDER_FLAG_TABLE_BIAS = 1,
  CANS_RENDER_FRAME_ORIENTATION_FLAG = 8,
  CANS_RENDER_DIRECTION_FIRST_FLAG = 0x0800,
  CANS_RENDER_DIRECTION_SECOND_FLAG = 0x1000,
  CANS_RENDER_DIRECTION_THIRD_FLAG = 0x2000,
  CANS_RENDER_DIRECTION_FOURTH_FLAG = 0x4000,
  CANS_RENDER_DIRECTION_MAXIMUM_FLAG = 0x8000,
  CANS_RENDER_DIRECTION_REVERSE_FLAG = 0x10,
  CANS_RENDER_DIRECTION_LIMIT = 7
};

#define CANS_RENDER_DIRECTION_FIRST_WEIGHT ((int)0x0002AAAAU)
#define CANS_RENDER_DIRECTION_SECOND_WEIGHT ((int)0x00055555U)
#define CANS_RENDER_DIRECTION_THIRD_WEIGHT ((int)0x000AAAAAU)
#define CANS_RENDER_DIRECTION_FOURTH_WEIGHT ((int)0x00155555U)
#define CANS_RENDER_DIRECTION_MAXIMUM_WEIGHT ((int)0x00200000U)

__inline
static
int
divide_cans_render_fixed_towards_zero(int value)
{
  if(value < 0)
    {
      value += CANS_RENDER_FIXED_ROUNDING_BIAS;
    }
  return value >> CANS_RENDER_FIXED_SHIFT;
}


__inline
static
int
divide_cans_render_power_towards_zero(int value,
                                      int shift)
{
  if(value < 0)
    {
      value += (1 << shift) - 1;
    }
  return value >> shift;
}


__inline
static
int
wrap_cans_render_angle(int angle)
{
  if(angle >= CANS_RENDER_ANGLE_WRAP_LIMIT)
    {
      angle -= CANS_RENDER_ANGLE_RANGE;
    }
  return angle;
}


static
int
select_cans_vertical_frame(const CelAnimationBinding   *binding,
                           const CansRenderOrientation *orientation)
{
  SignedDivisionResult division;
  int frame;
  int frame_scale;
  int frame_product;
  int variant_count;
  int vertical_extent;

  vertical_extent = binding->vertical_extent;
  if(vertical_extent == 0)
    {
      return 0;
    }

  variant_count = (signed char)binding->vertical_variant_count;
  if(vertical_extent < CANS_RENDER_SPECIAL_EXTENT_BASE)
    {
      division = signed_divide_with_remainder(
        (unsigned int)vertical_extent,
        (unsigned int)((variant_count & ~1) <<
                       CANS_RENDER_FRAME_SCALE_SHIFT));
      frame_scale = (int)division.quotient;
      frame_product = frame_scale *
                      (divide_cans_render_fixed_towards_zero(orientation->vertical) +
                       ((vertical_extent << CANS_RENDER_FIXED_SHIFT) >> 1));
      frame = divide_cans_render_power_towards_zero(
        frame_product, CANS_RENDER_FRAME_PRODUCT_SHIFT) +
              CANS_RENDER_FRAME_ROUNDING;
    }
  else if(variant_count == CANS_RENDER_EXTENT_TWO)
    {
      frame = divide_cans_render_power_towards_zero(
        orientation->vertical + CANS_RENDER_ANGLE_CENTER,
        CANS_RENDER_EXTENT_TWO_SHIFT);
    }
  else if(variant_count == CANS_RENDER_EXTENT_FOUR)
    {
      frame = divide_cans_render_power_towards_zero(
        orientation->vertical + CANS_RENDER_ANGLE_CENTER,
        CANS_RENDER_EXTENT_FOUR_SHIFT);
    }
  else if(variant_count == CANS_RENDER_EXTENT_EIGHT)
    {
      frame = divide_cans_render_power_towards_zero(
        orientation->vertical + CANS_RENDER_ANGLE_CENTER,
        CANS_RENDER_EXTENT_EIGHT_SHIFT);
    }
  else
    {
      frame = divide_cans_render_fixed_towards_zero(
        variant_count *
        (orientation->vertical + CANS_RENDER_ANGLE_CENTER));
    }

  frame >>= CANS_RENDER_FRAME_FRACTION_SHIFT;
  if(frame >= variant_count)
    {
      frame = variant_count - 1;
    }
  if(frame < 0)
    {
      frame = 0;
    }
  return frame;
}


__inline
static
int
select_cans_horizontal_column(const CelAnimationBinding   *binding,
                              const CansRenderOrientation *orientation)
{
  SignedDivisionResult division;
  int column;
  int column_scale;
  int horizontal_count;

  horizontal_count = (signed char)binding->horizontal_variant_count;
  division = signed_divide_with_remainder(
    (unsigned int)horizontal_count,
    (unsigned int)(binding->horizontal_extent <<
                   CANS_RENDER_COLUMN_SCALE_SHIFT));
  column_scale = (int)division.quotient;
  division = signed_divide_with_remainder(
    (unsigned int)column_scale,
    (unsigned int)(orientation->bank +
                   (binding->horizontal_extent <<
                    CANS_RENDER_COLUMN_EXTENT_SHIFT)));
  column = (signed char)division.quotient;

  if(column >= horizontal_count)
    {
      column = horizontal_count - 1;
    }
  else if(column < 0)
    {
      column = 0;
    }
  return column;
}


static
int
cans_render_direction_offset(int flags)
{
  int direction_offset;

  direction_offset = 0;
  if((flags & CANS_RENDER_DIRECTION_FIRST_FLAG) != 0)
    {
      direction_offset += CANS_RENDER_DIRECTION_FIRST_WEIGHT;
    }
  if((flags & CANS_RENDER_DIRECTION_SECOND_FLAG) != 0)
    {
      direction_offset += CANS_RENDER_DIRECTION_SECOND_WEIGHT;
    }
  if((flags & CANS_RENDER_DIRECTION_THIRD_FLAG) != 0)
    {
      direction_offset += CANS_RENDER_DIRECTION_THIRD_WEIGHT;
    }
  if((flags & CANS_RENDER_DIRECTION_FOURTH_FLAG) != 0)
    {
      direction_offset += CANS_RENDER_DIRECTION_FOURTH_WEIGHT;
    }
  if((flags & CANS_RENDER_DIRECTION_MAXIMUM_FLAG) != 0)
    {
      direction_offset += CANS_RENDER_DIRECTION_MAXIMUM_WEIGHT;
    }

  if(direction_offset > CANS_RENDER_DIRECTION_LIMIT)
    {
      direction_offset = CANS_RENDER_DIRECTION_LIMIT;
    }
  else if(direction_offset < -CANS_RENDER_DIRECTION_LIMIT)
    {
      direction_offset = -CANS_RENDER_DIRECTION_LIMIT;
    }
  if((flags & CANS_RENDER_DIRECTION_REVERSE_FLAG) != 0)
    {
      direction_offset = -direction_offset;
    }
  return direction_offset;
}


void
prepare_cans_animation_frame_render(CansAnimationRenderObject  *object,
                                    RoadProjectedRenderContext *render_context)
{
  const CelAnimationBinding *binding;
  CansAnimationFrameOutput frame_output;
  CansRenderOrientation orientation;
  const int *frame_flags;
  int frame;
  int column;
  int entry_index;
  int channel_index;
  int flags;
  int horizontal_variant_count;
  int binding_channel;
  int vertical_variant_count;

  binding = object->binding;
  binding_channel = 0;
  horizontal_variant_count = 0;
  vertical_variant_count = 0;
  if(binding != 0)
    {
      binding_channel = (signed char)binding->channel;
      horizontal_variant_count =
        (signed char)binding->horizontal_variant_count;
      vertical_variant_count =
        (signed char)binding->vertical_variant_count;
    }
  if(binding == 0 || binding->entry_index < 0 ||
     object->resources.cans_resource == 0 ||
     object->resources.animation_resource == 0 ||
     binding_channel < 0 || vertical_variant_count <= 0 ||
     (binding->uses_variant_grid != 0 &&
      (horizontal_variant_count <= 0 ||
       binding->frame_flags == 0)))
    {
      object->projected_cel.source_ccb = 0;
      object->projected_cel.horizontal_extent = 0;
      object->projected_cel.vertical_extent = 0;
      object->projected_cel.flags = 0;
      object->projected_cel.mapping_parameter = 0;
      return;
    }

  calculate_cans_render_orientation(object, &orientation, render_context);
  if(binding->frame_flags != 0 &&
     (binding->frame_flags[CANS_RENDER_FLAG_TABLE_BIAS] &
      CANS_RENDER_FRAME_ORIENTATION_FLAG) != 0)
    {
      orientation.vertical += CANS_RENDER_ANGLE_WRAP_LIMIT;
    }
  orientation.horizontal = wrap_cans_render_angle(orientation.horizontal);
  orientation.vertical = wrap_cans_render_angle(orientation.vertical);
  orientation.bank = wrap_cans_render_angle(orientation.bank);

  frame = select_cans_vertical_frame(binding, &orientation);
  column = 0;
  if(binding->uses_variant_grid == 0 ||
     (signed char)binding->horizontal_variant_count <= 1)
    {
      object->projected_cel.mapping_parameter = orientation.bank;
    }
  else
    {
      column = select_cans_horizontal_column(binding, &orientation);
      frame_flags = binding->frame_flags;
      flags = frame_flags[
        frame_flags[0] * frame + frame + column +
        CANS_RENDER_FLAG_TABLE_BIAS];
      object->projected_cel.mapping_parameter =
        orientation.bank - cans_render_direction_offset(flags);
    }

  entry_index = frame;
  channel_index = column;
  if(binding->uses_variant_grid == 0)
    {
      entry_index = 0;
      channel_index = frame;
    }

  decode_cans_animation_frame(
    &object->resources,
    entry_index + binding->entry_index,
    channel_index + (signed char)binding->channel,
    &frame_output);
  object->projected_cel.source_ccb = frame_output.ccb;
  object->projected_cel.horizontal_extent = frame_output.center_x;
  object->projected_cel.vertical_extent = frame_output.center_y;
  object->projected_cel.flags =
    (unsigned int)frame_output.animation_flags;
}


#undef CANS_RENDER_DIRECTION_MAXIMUM_WEIGHT
#undef CANS_RENDER_DIRECTION_FOURTH_WEIGHT
#undef CANS_RENDER_DIRECTION_THIRD_WEIGHT
#undef CANS_RENDER_DIRECTION_SECOND_WEIGHT
#undef CANS_RENDER_DIRECTION_FIRST_WEIGHT

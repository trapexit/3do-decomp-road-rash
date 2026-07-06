#include "display_cel_runtime.h"

static
int
decode_cel_height(int encoded_height)
{
  if(encoded_height >= 0)
    {
      return encoded_height & ROAD_CEL_SCALE_MASK;
    }
  return 1 << (encoded_height & DISPLAY_CEL_SOURCE_SHIFT_MASK);
}


static
int
divide_by_two_towards_zero(int value)
{
  return (value + (int)((unsigned int)value >>
                        (ROAD_INTEGER_BIT_COUNT - 1))) >> 1;
}


void
trim_cel_to_lower_half(CCB *cel)
{
  int source_row;
  int remaining_height;

  source_row = divide_by_two_towards_zero(
    decode_cel_height(cel->ccb_Height));
  cel->ccb_SourcePtr = select_cel_source_row(cel, source_row);

  remaining_height = cel->ccb_Height - 1;
  cel->ccb_Height = remaining_height;
  cel->ccb_PRE0 =
    (cel->ccb_PRE0 & ~PRE0_VCNT_MASK) |
    ROAD_PRE0_SHADE_BITS(decode_cel_height(remaining_height));
}

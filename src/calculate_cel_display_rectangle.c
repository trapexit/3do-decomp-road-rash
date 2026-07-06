#include "display_cel_runtime.h"

#define DECODE_CEL_EXTENT(encoded_extent) \
        ((encoded_extent) >= 0 \
         ? (int)(((uint32)(encoded_extent) << \
                  DISPLAY_CEL_EXTENT_SIGN_SHIFT) >> \
                 DISPLAY_CEL_EXTENT_SIGN_SHIFT) \
         : 1 << ((encoded_extent) & DISPLAY_CEL_EXTENT_EXPONENT_MASK))

void
calculate_cel_display_rectangle(const CCB        *cel,
                                DisplayRectangle *rectangle,
                                int               x,
                                int               y,
                                int               scale)
{
  int width;
  int height;

  if(cel == 0)
    {
      return;
    }

  width = DECODE_CEL_EXTENT(cel->ccb_Width);
  height = DECODE_CEL_EXTENT(cel->ccb_Height);

  if(scale == DISPLAY_CEL_SCALE_ONE)
    {
      rectangle->left = x;
      rectangle->top = y;
      rectangle->right = x + width;
      rectangle->bottom = y + height;
      return;
    }

  width >>= 1;
  height >>= 1;
  x += width;
  y += height;
  width = (int)(((uint32)(scale * width)) >> DISPLAY_CEL_SCALE_SHIFT);
  height = (int)(((uint32)(scale * height)) >> DISPLAY_CEL_SCALE_SHIFT);
  rectangle->left = x - width;
  rectangle->top = y - height;
  rectangle->right = x + width;
  rectangle->bottom = y + height;
}


#undef DECODE_CEL_EXTENT

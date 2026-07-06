#include "display_cel_runtime.h"

#define DECODE_CEL_EXTENT(encoded_extent) \
        ((encoded_extent) >= 0 \
         ? (int)(((uint32)(encoded_extent) << \
                  DISPLAY_CEL_EXTENT_SIGN_SHIFT) >> \
                 DISPLAY_CEL_EXTENT_SIGN_SHIFT) \
         : 1 << ((encoded_extent) & DISPLAY_CEL_EXTENT_EXPONENT_MASK))

#define ENCODED_CEL_SCALE_FACTOR(encoded_extent) \
        ((encoded_extent) >= 0 \
         ? (encoded_extent) >> DISPLAY_CEL_ENCODED_FACTOR_SHIFT \
         : (int)((uint32)DISPLAY_CEL_SCALE_ONE >> \
                 ((encoded_extent) & DISPLAY_CEL_EXTENT_EXPONENT_MASK)))

void
map_encoded_cel_to_rectangle(CCB                    *cel,
                             const DisplayRectangle *rectangle)
{
  int width;
  int height;
  int encoded_extent;
  int extent;
  int scale_factor;

  if(cel == 0)
    {
      return;
    }

  cel->ccb_XPos = rectangle->left << DISPLAY_COORDINATE_FRACTION_SHIFT;
  cel->ccb_YPos = rectangle->top << DISPLAY_COORDINATE_FRACTION_SHIFT;
  cel->ccb_HDY = 0;
  cel->ccb_VDX = 0;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;

  width = rectangle->right - rectangle->left;
  height = rectangle->bottom - rectangle->top;

  encoded_extent = cel->ccb_Width;
  extent = DECODE_CEL_EXTENT(encoded_extent);
  if(extent == width)
    {
      cel->ccb_HDX = DISPLAY_CEL_HDX_ONE;
    }
  else
    {
      scale_factor = ENCODED_CEL_SCALE_FACTOR(encoded_extent);
      cel->ccb_HDX = (width * scale_factor) >>
                     DISPLAY_CEL_HDX_FACTOR_SHIFT;
      cel->ccb_XPos += DISPLAY_CEL_SCALE_HALF;
    }

  encoded_extent = cel->ccb_Height;
  extent = DECODE_CEL_EXTENT(encoded_extent);
  if(extent == height)
    {
      cel->ccb_VDY = DISPLAY_CEL_SCALE_ONE;
      return;
    }

  scale_factor = ENCODED_CEL_SCALE_FACTOR(encoded_extent);
  cel->ccb_VDY = (height * scale_factor) >> DISPLAY_CEL_VDY_FACTOR_SHIFT;
  cel->ccb_YPos += DISPLAY_CEL_SCALE_HALF;
}


#undef DECODE_CEL_EXTENT
#undef ENCODED_CEL_SCALE_FACTOR

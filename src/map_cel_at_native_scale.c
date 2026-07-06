#include "display_cel_runtime.h"

#define CEL_HORIZONTAL_UNIT_SCALE 0x00100000
#define CEL_VERTICAL_UNIT_SCALE   0x00010000

int32 *
map_cel_at_native_scale(CCB *cel,
                        int  x,
                        int  y)
{
  if(cel == 0)
    {
      return 0;
    }

  cel->ccb_XPos =
    (int)((unsigned int)x << DISPLAY_COORDINATE_FRACTION_SHIFT);
  cel->ccb_YPos =
    (int)((unsigned int)y << DISPLAY_COORDINATE_FRACTION_SHIFT);
  cel->ccb_HDX = CEL_HORIZONTAL_UNIT_SCALE;
  cel->ccb_HDY = 0;
  cel->ccb_VDX = 0;
  cel->ccb_VDY = CEL_VERTICAL_UNIT_SCALE;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;
  return &cel->ccb_VDY;
}

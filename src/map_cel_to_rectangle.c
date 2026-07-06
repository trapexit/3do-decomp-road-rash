#include "display_cel_runtime.h"

int *
map_cel_to_rectangle(CCB                    *cel,
                     const DisplayRectangle *rectangle)
{
  if(cel == 0)
    {
      return 0;
    }

  cel->ccb_XPos = rectangle->left << DISPLAY_COORDINATE_FRACTION_SHIFT;
  cel->ccb_YPos = rectangle->top << DISPLAY_COORDINATE_FRACTION_SHIFT;
  cel->ccb_HDX = (rectangle->right - rectangle->left) <<
                 DISPLAY_RECTANGLE_HDX_SHIFT;
  cel->ccb_HDY = 0;
  cel->ccb_VDX = 0;
  cel->ccb_VDY = (rectangle->bottom - rectangle->top) <<
                 DISPLAY_RECTANGLE_VDY_SHIFT;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;
  return (int *)&cel->ccb_HDDY;
}

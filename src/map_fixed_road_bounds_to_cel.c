#include "display_cel_runtime.h"
#include "road_geometry.h"

int *
map_fixed_road_bounds_to_cel(CCB                 *cel,
                             const RoadCelBounds *bounds)
{
  int width_shift;
  int height_shift;

  if(cel == 0)
    {
      return 0;
    }

  cel->ccb_XPos = bounds->left;
  cel->ccb_YPos = bounds->top;
  cel->ccb_HDY = 0;
  cel->ccb_VDX = 0;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;

  width_shift = cel->ccb_Width & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  cel->ccb_HDX =
    ((bounds->right - bounds->left) <<
      DISPLAY_CEL_MAPPING_SCALE_SHIFT) >> width_shift;
  height_shift = cel->ccb_Height & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  cel->ccb_VDY = (bounds->bottom - bounds->top) >> height_shift;
  return (int *)&cel->ccb_VDY;
}

#include "display_cel_runtime.h"
#include "road_geometry.h"

int *
map_fixed_road_quad_to_cel(CCB            *cel,
                           const RoadQuad *quad)
{
  int width_shift;
  int height_shift;
  int combined_shift;
  int top_vertical_delta;

  if(cel == 0)
    {
      return 0;
    }

  width_shift = cel->ccb_Width & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  height_shift = cel->ccb_Height & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  combined_shift = width_shift + height_shift;

  cel->ccb_XPos = quad->top_left.x;
  cel->ccb_YPos = quad->top_left.y;

  top_vertical_delta = quad->top_left.y - quad->top_right.y;
  cel->ccb_HDX =
    ((quad->top_right.x - quad->top_left.x) <<
      DISPLAY_CEL_MAPPING_SCALE_SHIFT) >> width_shift;
  cel->ccb_HDY =
    ((quad->top_right.y - quad->top_left.y) <<
      DISPLAY_CEL_MAPPING_SCALE_SHIFT) >> width_shift;
  cel->ccb_VDX = 0;
  cel->ccb_VDY =
    (quad->bottom_left.y - quad->top_left.y) >> height_shift;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY =
    ((quad->bottom_right.y - quad->bottom_left.y +
      top_vertical_delta) << DISPLAY_CEL_MAPPING_SCALE_SHIFT) >>
    combined_shift;

  return (int *)&cel->ccb_HDDX;
}

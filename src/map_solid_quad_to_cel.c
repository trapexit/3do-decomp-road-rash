#include "display_cel_runtime.h"

enum SolidQuadMappingValue
{
  SOLID_QUAD_HORIZONTAL_DELTA_SHIFT = 3,
  SOLID_QUAD_VERTICAL_DELTA_SHIFT = 1,
  SOLID_QUAD_PERSPECTIVE_DELTA_SHIFT = 2
};

int *
map_solid_quad_to_cel(CCB               *cel,
                      const DisplayQuad *quad)
{
  int horizontal_delta;
  int vertical_delta;

  if(cel == 0)
    {
      return 0;
    }

  cel->ccb_XPos = quad->top_left.x;
  cel->ccb_YPos = quad->top_left.y;

  horizontal_delta = quad->top_left.x - quad->top_right.x;
  vertical_delta = quad->top_left.y - quad->top_right.y;
  cel->ccb_HDX =
    -horizontal_delta << SOLID_QUAD_HORIZONTAL_DELTA_SHIFT;
  cel->ccb_HDY =
    -vertical_delta << SOLID_QUAD_HORIZONTAL_DELTA_SHIFT;
  cel->ccb_VDX =
    (quad->bottom_left.x - quad->top_left.x) >>
    SOLID_QUAD_VERTICAL_DELTA_SHIFT;
  cel->ccb_VDY =
    (quad->bottom_left.y - quad->top_left.y) >>
    SOLID_QUAD_VERTICAL_DELTA_SHIFT;
  cel->ccb_HDDX =
    ((quad->bottom_right.x - quad->bottom_left.x) +
     horizontal_delta) << SOLID_QUAD_PERSPECTIVE_DELTA_SHIFT;
  cel->ccb_HDDY =
    ((quad->bottom_right.y - quad->bottom_left.y) + vertical_delta) <<
      SOLID_QUAD_PERSPECTIVE_DELTA_SHIFT;
  return (int *)&cel->ccb_HDDX;
}

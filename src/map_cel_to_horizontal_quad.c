#include "display_cel_runtime.h"
#include "road_geometry.h"

void
map_cel_to_horizontal_quad(CCB            *cel,
                           const RoadQuad *quad)
{
  int width_shift;
  int height_shift;
  int combined_shift;
  int top_horizontal_delta;

  if(cel == 0)
    {
      return;
    }

  width_shift = cel->ccb_Width & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  height_shift = cel->ccb_Height & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  combined_shift = width_shift + height_shift;

  cel->ccb_XPos = quad->top_left.x;
  cel->ccb_YPos = quad->top_left.y;

  top_horizontal_delta = quad->top_left.x - quad->top_right.x;
  cel->ccb_HDX =
    ((-top_horizontal_delta) << DISPLAY_CEL_MAPPING_SCALE_SHIFT) >>
    width_shift;
  cel->ccb_HDY = 0;
  cel->ccb_VDX =
    (quad->bottom_left.x - quad->top_left.x) >> height_shift;
  cel->ccb_VDY =
    (quad->bottom_left.y - quad->top_left.y) >> height_shift;
  cel->ccb_HDDX =
    ((quad->bottom_right.x - quad->bottom_left.x +
      top_horizontal_delta) << DISPLAY_CEL_MAPPING_SCALE_SHIFT) >>
    combined_shift;
  cel->ccb_HDDY = 0;
  cel->ccb_Flags |= CCB_LDSIZE | CCB_LDPRS;
}


void
map_cel_to_perspective_quad(CCB            *cel,
                            const RoadQuad *quad)
{
  int width_shift;
  int height_shift;
  int combined_shift;
  int mapping_shift;
  int top_horizontal_delta;
  int top_vertical_delta;
  int perspective_horizontal_delta;
  int perspective_vertical_delta;

  if(cel == 0)
    {
      return;
    }

  cel->ccb_XPos = quad->top_left.x;
  cel->ccb_YPos = quad->top_left.y;

  top_horizontal_delta = quad->top_left.x - quad->top_right.x;
  top_vertical_delta = quad->top_left.y - quad->top_right.y;
  width_shift = cel->ccb_Width & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  if(width_shift >= DISPLAY_CEL_MAPPING_SCALE_SHIFT)
    {
      mapping_shift = width_shift - DISPLAY_CEL_MAPPING_SCALE_SHIFT;
      cel->ccb_HDX = (-top_horizontal_delta) >> mapping_shift;
      cel->ccb_HDY = (-top_vertical_delta) >> mapping_shift;
    }
  else
    {
      mapping_shift = DISPLAY_CEL_MAPPING_SCALE_SHIFT - width_shift;
      cel->ccb_HDX = (-top_horizontal_delta) << mapping_shift;
      cel->ccb_HDY = (-top_vertical_delta) << mapping_shift;
    }

  height_shift = cel->ccb_Height & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  combined_shift = width_shift + height_shift;
  cel->ccb_VDX =
    (quad->bottom_left.x - quad->top_left.x) >> height_shift;
  cel->ccb_VDY =
    (quad->bottom_left.y - quad->top_left.y) >> height_shift;

  perspective_horizontal_delta =
    quad->bottom_right.x - quad->bottom_left.x +
    top_horizontal_delta;
  perspective_vertical_delta =
    quad->bottom_right.y - quad->bottom_left.y + top_vertical_delta;
  if(combined_shift >= DISPLAY_CEL_MAPPING_SCALE_SHIFT)
    {
      mapping_shift = combined_shift - DISPLAY_CEL_MAPPING_SCALE_SHIFT;
      cel->ccb_HDDX = perspective_horizontal_delta >> mapping_shift;
      cel->ccb_HDDY = perspective_vertical_delta >> mapping_shift;
    }
  else
    {
      mapping_shift = DISPLAY_CEL_MAPPING_SCALE_SHIFT - combined_shift;
      cel->ccb_HDDX = perspective_horizontal_delta << mapping_shift;
      cel->ccb_HDDY = perspective_vertical_delta << mapping_shift;
    }
  cel->ccb_Flags |= CCB_LDSIZE | CCB_LDPRS;
}

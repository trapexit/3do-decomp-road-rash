#include "road_surface_cel_selection.h"

#include "bss_early_data.h"
#include "cel_dimension_resource.h"

enum RoadStripCelValue
{
  ROAD_STRIP_APPROXIMATION_HALF_SHIFT = 1
};

CCB *
select_road_strip_cel(RoadRenderSide *road_side,
                      RoadSide        side,
                      const RoadQuad *quad,
                      int             resource_row)
{
  void *resource;
  int dx;
  int dy;
  int combined_span;
  int top_span;
  int side_span;

  resource = road_side->texture_cache->surface_sources[resource_row][side];
  if(resource == 0)
    {
      return 0;
    }

  if(resource_row == 0)
    {
      dx = quad->top_right.x - quad->top_left.x;
      if(dx < 0)
        {
          dx = -dx;
        }
      dy = quad->top_right.y - quad->top_left.y;
      if(dy < 0)
        {
          dy = -dy;
        }
      combined_span = dx + dy;
      if(dx < dy)
        {
          dy = dx;
        }
      top_span =
        (combined_span - (dy >> ROAD_STRIP_APPROXIMATION_HALF_SHIFT)) >>
        ROAD_FIXED_FRACTION_BITS;

      dx = quad->top_right.x - quad->bottom_right.x;
      if(dx < 0)
        {
          dx = -dx;
        }
      dy = quad->top_right.y - quad->bottom_right.y;
      if(dy < 0)
        {
          dy = -dy;
        }
      combined_span = dx + dy;
      if(dx >= dy)
        {
          dx = dy;
        }
      side_span =
        (combined_span - (dx >> ROAD_STRIP_APPROXIMATION_HALF_SHIFT)) >>
        ROAD_FIXED_FRACTION_BITS;
    }
  else
    {
      top_span = quad->top_right.x - quad->top_left.x;
      if(top_span < 0)
        {
          top_span = -top_span;
        }

      side_span = quad->bottom_left.y - quad->top_left.y;
      if(side_span < 0)
        {
          side_span = -side_span;
        }

      top_span >>= ROAD_FIXED_FRACTION_BITS;
      side_span >>= ROAD_FIXED_FRACTION_BITS;
    }

  return select_cel_for_dimensions(resource, top_span, side_span);
}

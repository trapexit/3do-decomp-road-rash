#include "road_geometry.h"

#include "bss_early_data.h"

enum RoadQuadVisibilityValue
{
  ROAD_QUAD_VISIBILITY_DIRECTION_MASK = 0xFF
};

int
road_quad_is_horizontally_visible(const RoadRenderSide *road_side,
                                  const RoadQuad       *quad,
                                  int                   reverse_direction)
{
  (void)road_side;

  if((reverse_direction & ROAD_QUAD_VISIBILITY_DIRECTION_MASK) != 0)
    {
      if(quad->top_right.x >= ROAD_SCREEN_RIGHT &&
         quad->bottom_right.x >= ROAD_SCREEN_RIGHT)
        {
          return 0;
        }
      if(quad->top_left.x <= 0 && quad->bottom_left.x <= 0)
        {
          return 0;
        }
    }
  else
    {
      if(quad->top_left.x >= ROAD_SCREEN_RIGHT &&
         quad->bottom_left.x >= ROAD_SCREEN_RIGHT)
        {
          return 0;
        }
      if(quad->top_right.x <= 0 && quad->bottom_right.x <= 0)
        {
          return 0;
        }
    }
  return 1;
}

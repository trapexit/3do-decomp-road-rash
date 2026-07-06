#include "road_surface_cel_selection.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "rw_semantic_data.h"

enum RoadSurfaceStyleValue
{
  ROAD_SURFACE_PROJECTED_COORDINATE_BITS = 16,
  ROAD_SURFACE_CEL_LITERAL_SHIFT = 23,
  ROAD_SURFACE_TRANSITION_FLAG_MASK =
    ROAD_SURFACE_FLAG_REVERSED_MARGIN |
    ROAD_SURFACE_FLAG_DEPTH_ADJUSTED |
    ROAD_SURFACE_FLAG_BLEND_TRANSITION,
  ROAD_SURFACE_DYNAMIC_THRESHOLD_COUNT = 2,
  ROAD_SURFACE_STYLE_NEAREST = 0,
  ROAD_SURFACE_STYLE_NEAR = 1,
  ROAD_SURFACE_STYLE_MIDDLE_WIDE = 2,
  ROAD_SURFACE_STYLE_MIDDLE_NARROW = 3,
  ROAD_SURFACE_STYLE_FAR_WIDE = 4,
  ROAD_SURFACE_STYLE_VERY_FAR_WIDE = 5,
  ROAD_SURFACE_STYLE_FAR_NARROW = 6,
  ROAD_SURFACE_STYLE_VERY_FAR_NARROW = 7,
  ROAD_SURFACE_STYLE_DISTANT = 8,
  ROAD_SURFACE_DISTANT_HEIGHT = 1,
  ROAD_SURFACE_VERY_FAR_HEIGHT = 4,
  ROAD_SURFACE_FAR_HEIGHT = 8,
  ROAD_SURFACE_MIDDLE_HEIGHT = 0x0C,
  ROAD_SURFACE_NEAREST_HEIGHT = 0x1E,
  ROAD_SURFACE_FAR_WIDTH = 0x0D,
  ROAD_SURFACE_MIDDLE_WIDTH = 0x19
};

int
select_road_surface_style(RoadRenderSide *road_side,
                          const RoadQuad *quad,
                          int             depth_multiplier)
{
  RoadSegmentLaneRuntime *lane;
  CCB *dynamic_cel;
  unsigned int surface_flags;
  int projected_height;
  int projected_width;
  int packed_height;
  int cel_height;
  int style_index;

  projected_height = quad->bottom_left.y - quad->top_left.y;
  projected_height >>= ROAD_SURFACE_PROJECTED_COORDINATE_BITS;
  if(projected_height < 0)
    {
      projected_height = -projected_height;
    }
  if(depth_multiplier != 0)
    {
      projected_height *= depth_multiplier;
      if(projected_height <= depth_multiplier)
        {
          projected_height = depth_multiplier + 1;
        }
    }

  lane = road_side->lane;
  if((lane->topology_flags & ROAD_TOPOLOGY_DYNAMIC_CEL_FLAG) != 0)
    {
      dynamic_cel = (CCB *)road_surface_dynamic_cels[0];
      if(dynamic_cel != 0)
        {
          packed_height = dynamic_cel->ccb_Height;
          if(packed_height >= 0)
            {
              cel_height =
                (int)(((unsigned int)packed_height <<
                       ROAD_SURFACE_CEL_LITERAL_SHIFT) >>
                      ROAD_SURFACE_CEL_LITERAL_SHIFT);
            }
          else
            {
              cel_height =
                1 << (packed_height & DISPLAY_CEL_SOURCE_SHIFT_MASK);
            }
          return cel_height > projected_height;
        }
    }

  surface_flags = lane->surface_flags;
  if(surface_flags != 0 &&
     (surface_flags & ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG) == 0 &&
     (surface_flags & ROAD_SURFACE_TRANSITION_FLAG_MASK) != 0)
    {
      style_index = 0;
      while(style_index < ROAD_SURFACE_DYNAMIC_THRESHOLD_COUNT)
        {
          if(road_surface_thresholds[style_index] <= projected_height)
            {
              return style_index;
            }
          style_index++;
        }
      return style_index;
    }

  projected_width = quad->top_left.x - quad->top_right.x;
  projected_width >>= ROAD_SURFACE_PROJECTED_COORDINATE_BITS;
  if(projected_width < 0)
    {
      projected_width = -projected_width;
    }
  if(projected_height <= ROAD_SURFACE_DISTANT_HEIGHT)
    {
      return ROAD_SURFACE_STYLE_DISTANT;
    }
  if(projected_height <= ROAD_SURFACE_VERY_FAR_HEIGHT)
    {
      if(projected_width >= ROAD_SURFACE_FAR_WIDTH)
        {
          return ROAD_SURFACE_STYLE_VERY_FAR_WIDE;
        }
      return ROAD_SURFACE_STYLE_VERY_FAR_NARROW;
    }
  if(projected_height < ROAD_SURFACE_FAR_HEIGHT)
    {
      if(projected_width >= ROAD_SURFACE_FAR_WIDTH)
        {
          return ROAD_SURFACE_STYLE_FAR_WIDE;
        }
      return ROAD_SURFACE_STYLE_FAR_NARROW;
    }
  if(projected_height < ROAD_SURFACE_MIDDLE_HEIGHT)
    {
      if(projected_width >= ROAD_SURFACE_MIDDLE_WIDTH)
        {
          return ROAD_SURFACE_STYLE_MIDDLE_WIDE;
        }
      return ROAD_SURFACE_STYLE_MIDDLE_NARROW;
    }
  if(projected_height >= ROAD_SURFACE_NEAREST_HEIGHT)
    {
      return ROAD_SURFACE_STYLE_NEAREST;
    }
  return ROAD_SURFACE_STYLE_NEAR;
}

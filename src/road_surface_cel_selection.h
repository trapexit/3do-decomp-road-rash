#ifndef ROADRASH_ROAD_SURFACE_CEL_SELECTION_H
#define ROADRASH_ROAD_SURFACE_CEL_SELECTION_H

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

struct RoadRenderSide;

enum RoadSurfaceCelFlag
{
  ROAD_TOPOLOGY_DYNAMIC_CEL_FLAG = 0x20,
  ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG = 0x40
};

CCB *
select_road_surface_cel(struct RoadRenderSide *road_side,
                        RoadSide               side,
                        const RoadQuad        *quad,
                        int                    resource_row);
CCB *
select_road_strip_cel(struct RoadRenderSide *road_side,
                      RoadSide               side,
                      const RoadQuad        *quad,
                      int                    resource_row);
int
select_road_surface_style(struct RoadRenderSide *road_side,
                          const RoadQuad        *quad,
                          int                    depth_multiplier);
int
render_subdivided_road_surface_strips(struct RoadRenderSide *road_side);
int
render_road_surface_strips(struct RoadRenderSide *road_side);

enum RoadSurfaceCelSelectionValue
{
  ROAD_WIDTH_FRACTION_BITS = 8,
  ROAD_WIDTH_FRACTION_MASK = 0xFF,
  ROAD_WIDTH_ROUNDING_BIAS = 0xFF,
  ROAD_SURFACE_CEL_PARTIAL_VARIANT = 0,
  ROAD_SURFACE_CEL_FULL_VARIANT = 1,
  ROAD_SURFACE_CEL_JOIN_VARIANT = 2
};

__inline
static
int
road_width_whole_units(int value)
{
  if(value < 0)
    {
      value += ROAD_WIDTH_ROUNDING_BIAS;
    }
  return value >> ROAD_WIDTH_FRACTION_BITS;
}


__inline
static
int
select_strip_variant(int left_width,
                     int right_width,
                     int strip_index)
{
  int rounded_left_width;

  if(strip_index <= 0)
    {
      return strip_index;
    }

  rounded_left_width = road_width_whole_units(
    left_width + ROAD_WIDTH_ROUNDING_BIAS);
  if(strip_index == rounded_left_width)
    {
      return ROAD_SURFACE_CEL_JOIN_VARIANT;
    }

  if((left_width & ROAD_WIDTH_FRACTION_MASK) != 0 &&
     strip_index <= 1)
    {
      return ROAD_SURFACE_CEL_PARTIAL_VARIANT;
    }
  if((right_width & ROAD_WIDTH_FRACTION_MASK) == 0)
    {
      return ROAD_SURFACE_CEL_FULL_VARIANT;
    }

  return strip_index < rounded_left_width +
         road_width_whole_units(right_width);
}


__inline
static
CCB *
select_road_surface_strip_cel(RoadRenderSide *road_side,
                              int             surface_style,
                              int             left_width,
                              int             right_width,
                              int             strip_index)
{
  RoadSegmentLaneRuntime *lane;
  unsigned int surface_flags;
  int cel_variant;
  CCB *cel;

  lane = road_side->lane;
  if((lane->topology_flags & ROAD_TOPOLOGY_DYNAMIC_CEL_FLAG) != 0 &&
     road_surface_dynamic_cels[0] != 0)
    {
      cel = (CCB *)road_surface_dynamic_cels[surface_style];
    }
  else
    {
      cel_variant = select_strip_variant(left_width, right_width,
                                         strip_index);
      surface_flags = lane->surface_flags;
      if(surface_flags == 0 ||
         (surface_flags & ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG) != 0)
        {
          cel = (CCB *)gRoadSurfaceCelVariants[surface_style][cel_variant];
        }
      else if((surface_flags &
               (ROAD_SURFACE_FLAG_REVERSED_MARGIN |
                ROAD_SURFACE_FLAG_DEPTH_ADJUSTED)) != 0)
        {
          cel = (CCB *)road_surface_cel_variants[surface_style][cel_variant];
        }
      else if((surface_flags &
               ROAD_SURFACE_FLAG_BLEND_TRANSITION) != 0)
        {
          cel = (CCB *)road_surface_palette_variants[surface_style];
        }
      else
        {
          cel = (CCB *)gRoadSurfaceCelVariants[surface_style][cel_variant];
        }
    }

  if(cel == 0)
    {
      cel = display_runtime_globals.fallback_ccb;
    }
  return cel;
}

#endif

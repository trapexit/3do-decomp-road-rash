#include "bss_early_data.h"
#include "bss_late_data.h"
#include "cel_dimension_resource.h"
#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RoadsideOverhangValue
{
  ROADSIDE_OVERHANG_PIXC = 6,
  ROADSIDE_OVERHANG_DIVISION_SHIFT = 4
};

int
render_roadside_neighbor_overhang(RoadRenderSide *road_side,
                                  RoadSide        side)
{
  RoadRenderSide *neighbor;
  RoadSegmentLaneRuntime *neighbor_lane;
  RoadRenderPoint *neighbor_near;
  RoadRenderPoint *neighbor_shoulder;
  DisplayAxisAlignedCelMapping mapping;
  RoadFixed span;
  RoadFixed bound;
  RoadFixed horizontal_step;
  CCB *cel;
  int source_shift;
  int visible_rows;

  if((road_side->lane->surface_flags &
      ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
    {
      return (int)road_side;
    }

  neighbor = road_side->next_sides[side];
  if(neighbor == 0)
    {
      return (int)road_side;
    }

  neighbor_lane = neighbor->lane;
  if(neighbor_lane->geometry_mode != ROAD_GEOMETRY_MODE_TEXTURED)
    {
      return (int)road_side;
    }

  neighbor_near = &neighbor->mode_geometry[
    ROAD_MODE_GEOMETRY_LEFT_NEAR + side];
  neighbor_shoulder = &neighbor->lane_edges[
    ROAD_SHOULDER_EDGE_LEFT + side];
  span = neighbor_near->y;
  bound = road_side->mode_geometry[
    ROAD_MODE_GEOMETRY_LEFT_FAR + side].y;
  if(span - bound <= 0)
    {
      return bound;
    }

  mapping.x = neighbor_shoulder->x;
  mapping.y = neighbor->lane_edges[side].y - span;
  horizontal_step = neighbor_near->x;

  if(side == ROAD_SIDE_LEFT)
    {
      if(horizontal_step >= 0)
        {
          return mapping.y;
        }
      if(mapping.x <= 0)
        {
          return mapping.x;
        }
      cel = select_cel_for_dimensions(
        neighbor->texture_cache->surface_sources[0][side],
        (-horizontal_step) >> ROAD_FIXED_FRACTION_BITS,
        span >> ROAD_FIXED_FRACTION_BITS);
    }
  else
    {
      if(horizontal_step <= 0)
        {
          return mapping.y;
        }
      if(mapping.x >= ROAD_SCREEN_RIGHT)
        {
          return mapping.x;
        }
      cel = select_cel_for_dimensions(
        neighbor->texture_cache->surface_sources[0][side],
        horizontal_step >> ROAD_FIXED_FRACTION_BITS,
        span >> ROAD_FIXED_FRACTION_BITS);
    }

  if(cel == 0)
    {
      cel = display_runtime_globals.fallback_ccb;
    }

  source_shift = cel->ccb_Width & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  mapping.horizontal_delta = horizontal_step >> source_shift;
  source_shift = cel->ccb_Height & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  mapping.vertical_delta = (span + ROAD_FIXED_ONE) >> source_shift;
  cel->ccb_PIXC = ROADSIDE_OVERHANG_PIXC;

  cel = append_axis_aligned_cel(cel, &mapping);
  visible_rows = signed_divide_with_remainder(
    span >> ROADSIDE_OVERHANG_DIVISION_SHIFT,
    ((span - bound) >> ROADSIDE_OVERHANG_DIVISION_SHIFT) <<
      source_shift).quotient + 1;
  if((1 << source_shift) <= visible_rows)
    {
      return visible_rows;
    }

  cel->ccb_PRE0 =
    (cel->ccb_PRE0 & ~PRE0_VCNT_MASK) |
    ROAD_PRE0_SHADE_BITS(visible_rows);
  return cel->ccb_PRE0;
}

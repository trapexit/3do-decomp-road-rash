#include "bss_early_data.h"
#include "cel_dimension_resource.h"
#include "road_geometry.h"
#include "road_surface_asset_panic.h"
#include "rw_semantic_data.h"

CCB *
select_cached_side_cel(RoadRenderSide *road_side,
                       RoadSide        side,
                       const RoadQuad *quad)
{
  int delta_x;
  int delta_y;
  CCB *cel;

  delta_x = quad->top_right.x - quad->top_left.x;
  if(delta_x < 0)
    {
      delta_x = -delta_x;
    }

  delta_y = quad->bottom_right.y - quad->top_right.y;
  delta_x >>= ROAD_FIXED_FRACTION_BITS;
  delta_y >>= ROAD_FIXED_FRACTION_BITS;

  cel = select_cel_for_dimensions(
    road_side->texture_cache->surface_sources[0][side],
    delta_x, delta_y);
  if(cel == 0)
    {
      panic_missing_road_surface_asset(
        road_side, side, road_side->lane->surface_resource_ids[side],
        ROAD_SURFACE_FAMILY_GROUP, -1, delta_x, delta_y,
        ROAD_SURFACE_ASSET_DIMENSION_SELECTION_FAILED);
      return 0;
    }
  return cel;
}

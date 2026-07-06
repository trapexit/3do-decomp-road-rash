#include "bss_early_data.h"
#include "bss_late_data.h"
#include "family_resource_runtime.h"
#include "road_geometry.h"

void
populate_raised_road_surface_resource_cache(RoadRenderSide *road_side)
{
  int resource_variant;
  RoadSide side;

  resource_variant = 0;
  do
    {
      side = ROAD_SIDE_LEFT;
      do
        {
          road_side->texture_cache->surface_sources
          [resource_variant][side] =
            (void *)lookup_family_resource_child_for_render(
              road_side->lane->surface_resource_ids[side],
              ROAD_SURFACE_FAMILY_GROUP, resource_variant);
          side++;
        } while(side < ROAD_SIDE_COUNT);
      resource_variant++;
    } while(resource_variant <
            ROAD_RAISED_SURFACE_RESOURCE_VARIANT_COUNT);
}

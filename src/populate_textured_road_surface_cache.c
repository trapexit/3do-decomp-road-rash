#include "bss_early_data.h"
#include "bss_late_data.h"
#include "cel_dimension_resource.h"
#include "family_resource_runtime.h"
#include "road_geometry.h"
#include "road_surface_asset_panic.h"
#include "rw_semantic_data.h"

int
is_cel_first_pixel_transparent(CCB *cel);

void
populate_textured_road_surface_cache(RoadSegmentLaneRuntime *lane,
                                     RoadRenderSide         *road_side)
{
  RoadSide side;
  int profile_height;
  int family_id;
  int group_index;
  int selector;
  CelDimensionResource *resource;
  CCB *probe_cel;

  side = ROAD_SIDE_LEFT;
  do
    {
      selector = lane->surface_resource_ids[side];
      if(selector == ROAD_SURFACE_RESOURCE_MISSING)
        {
          road_side->texture_cache->surface_sources[0][side] = 0;
          profile_height = 0;
        }
      else
        {
          group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
          family_id = family_resource_globals
                      .resident_family_ids[group_index];
          if(family_id <= 0 ||
             family_resource_generations[group_index] == 0)
            {
              panic_missing_road_surface_asset(
                road_side, side, selector, ROAD_SURFACE_FAMILY_GROUP,
                -1, 0, 0, ROAD_SURFACE_ASSET_FAMILY_UNAVAILABLE);
              return;
            }
          else
            {
              resource = (CelDimensionResource *)
                         lookup_family_resource_entry_for_render(
                selector, ROAD_SURFACE_FAMILY_GROUP);
              if(resource != 0)
                {
                  road_side->texture_cache->surface_sources[0][side] =
                    resource;
                  profile_height =
                    (int)resource->dimension_table_word_offset <<
                      ROAD_SURFACE_RESOURCE_HEIGHT_SHIFT;
                  if(profile_height < ROAD_SURFACE_RESOURCE_MIN_HEIGHT)
                    {
                      profile_height = 0;
                      lane->surface_resource_ids[side] =
                        ROAD_SURFACE_RESOURCE_MISSING;
                    }
                  else
                    {
                      probe_cel = select_cel_for_dimensions(
                        resource, ROAD_SURFACE_RESOURCE_SCALE_PROBE,
                        ROAD_SURFACE_RESOURCE_SCALE_PROBE);
                      if(probe_cel == 0)
                        {
                          panic_missing_road_surface_asset(
                            road_side, side, selector,
                            ROAD_SURFACE_FAMILY_GROUP, -1,
                            ROAD_SURFACE_RESOURCE_SCALE_PROBE,
                            ROAD_SURFACE_RESOURCE_SCALE_PROBE,
                            ROAD_SURFACE_ASSET_DIMENSION_SELECTION_FAILED);
                          return;
                        }
                      lane->surface_profile[ROAD_SIDE_COUNT + side].y =
                        is_cel_first_pixel_transparent(probe_cel);
                    }
                }
              else
                {
                  panic_missing_road_surface_asset(
                    road_side, side, selector, ROAD_SURFACE_FAMILY_GROUP,
                    -1, 0, 0, ROAD_SURFACE_ASSET_ENTRY_MISSING);
                  return;
                }
            }
        }

      lane->surface_profile[side].y = profile_height;
      side++;
    } while(side <= ROAD_SIDE_RIGHT);
}

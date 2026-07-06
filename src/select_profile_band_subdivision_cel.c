#include "cel_dimension_resource.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RoadProfileCelConstant
{
  ROAD_PROFILE_CEL_RESOURCE_GROUP = 1,
  ROAD_PROFILE_CEL_MINIMUM_SPAN = 4
};

int
lookup_family_resource_child_for_render(int inventory,
                                        int item_type,
                                        int slot);

static
int
absolute_dimension(int value)
{
  return value < 0 ? -value : value;
}


static
int
approximate_span(int dx,
                 int dy)
{
  int smaller;

  smaller = dx < dy ? dx : dy;
  return (dx + dy - (smaller >> 1)) >> ROAD_FIXED_FRACTION_BITS;
}


CCB *
select_profile_band_subdivision_cel(RoadRenderSide *road_side,
                                    RoadSide        side,
                                    const RoadQuad *quad,
                                    int             strip_index)
{
  CCB **cel_slot;
  int first_span;
  int second_span;

  cel_slot = (CCB **)&road_side->texture_cache->surface_sources[
    strip_index][side];
  if(*cel_slot == 0)
    {
      *cel_slot = (CCB *)lookup_family_resource_child_for_render(
        road_side->lane->surface_resource_ids[side],
        ROAD_PROFILE_CEL_RESOURCE_GROUP, strip_index);
      if(*cel_slot == 0)
        {
          return 0;
        }
    }

  first_span = approximate_span(
    absolute_dimension(quad->top_right.x - quad->top_left.x),
    absolute_dimension(quad->top_right.y - quad->top_left.y));
  second_span = approximate_span(
    absolute_dimension(quad->bottom_right.x - quad->top_right.x),
    absolute_dimension(quad->bottom_right.y - quad->top_right.y));
  if(second_span < ROAD_PROFILE_CEL_MINIMUM_SPAN)
    {
      second_span = ROAD_PROFILE_CEL_MINIMUM_SPAN;
    }

  return select_cel_for_dimensions(
    *cel_slot, first_span, second_span);
}

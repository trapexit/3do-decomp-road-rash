
#include "road_surface_cel_selection.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "cel_dimension_resource.h"
#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

enum RoadSurfaceDimensionValue
{
  ROAD_SURFACE_APPROXIMATION_HALF_SHIFT = 1
};

CCB *
select_road_surface_cel(RoadRenderSide *road_side,
                        RoadSide        side,
                        const RoadQuad *quad,
                        int             resource_row)
{
  void **resource_slot;
  void *resource;
  int right_dx;
  int right_dy;
  int right_span;
  int left_dx;
  int left_dy;
  int left_span;
  int top_dx;
  int top_dy;
  int top_span;
  CCB *cel;

  resource_slot = &road_side->texture_cache->surface_sources[
    resource_row][side];
  resource = *resource_slot;
  if(resource == 0)
    {
      resource = (void *)lookup_family_resource_child_for_render(
        road_side->lane->surface_resource_ids[side],
        ROAD_SURFACE_FAMILY_GROUP, resource_row);
      *resource_slot = resource;
      if(resource == 0)
        {
          return display_runtime_globals.fallback_ccb;
        }
    }

  right_dx = quad->bottom_right.x - quad->top_right.x;
  if(right_dx < 0)
    {
      right_dx = -right_dx;
    }
  right_dy = quad->bottom_right.y - quad->top_right.y;
  if(right_dy < 0)
    {
      right_dy = -right_dy;
    }
  right_span = right_dx + right_dy;
  if(right_dx < right_dy)
    {
      right_dy = right_dx;
    }
  right_span -= right_dy >> ROAD_SURFACE_APPROXIMATION_HALF_SHIFT;

  left_dx = quad->bottom_left.x - quad->top_left.x;
  if(left_dx < 0)
    {
      left_dx = -left_dx;
    }
  left_dy = quad->bottom_left.y - quad->top_left.y;
  if(left_dy < 0)
    {
      left_dy = -left_dy;
    }
  left_span = left_dx + left_dy;
  if(left_dx < left_dy)
    {
      left_dy = left_dx;
    }
  left_span -= left_dy >> ROAD_SURFACE_APPROXIMATION_HALF_SHIFT;

  if(right_span < left_span)
    {
      right_span = left_span;
    }

  top_dx = quad->top_right.x - quad->top_left.x;
  if(top_dx < 0)
    {
      top_dx = -top_dx;
    }
  top_dy = quad->top_right.y - quad->top_left.y;
  if(top_dy < 0)
    {
      top_dy = -top_dy;
    }
  top_span = top_dx + top_dy;
  if(top_dx < top_dy)
    {
      top_dy = top_dx;
    }
  top_span -= top_dy >> ROAD_SURFACE_APPROXIMATION_HALF_SHIFT;

  cel = select_cel_for_dimensions(
    resource, top_span >> ROAD_FIXED_FRACTION_BITS,
    right_span >> ROAD_FIXED_FRACTION_BITS);
  if(cel == 0)
    {
      return display_runtime_globals.fallback_ccb;
    }
  return cel;
}

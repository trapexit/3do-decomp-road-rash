#include "cel_dimension_resource.h"
#include "family_resource_runtime.h"
#include "road_geometry.h"

enum CachedDescriptionCelConstant
{
  CACHED_DESCRIPTION_CEL_FAMILY_INDEX = 5
};

char *
select_cached_description_cel(char **cache_slots,
                              char  *description,
                              int    child_index,
                              int    side,
                              int   *quad)
{
  const unsigned char *resource_selectors;
  const RoadQuad *typed_quad;
  int x_delta;
  int y_delta;

  resource_selectors = (const unsigned char *)(const void *)description;
  typed_quad = (const RoadQuad *)(const void *)quad;

  if(cache_slots[side] == 0)
    {
      cache_slots[side] = (char *)
                          lookup_family_resource_child_for_render(
        resource_selectors[side],
        CACHED_DESCRIPTION_CEL_FAMILY_INDEX, child_index);
      if(cache_slots[side] == 0)
        {
          return 0;
        }
    }

  x_delta = typed_quad->top_right.x - typed_quad->top_left.x;
  if(x_delta < 0)
    {
      x_delta = -x_delta;
    }

  y_delta = typed_quad->bottom_left.y - typed_quad->top_left.y;
  if(y_delta < 0)
    {
      y_delta = -y_delta;
    }

  return (char *)select_cel_for_dimensions(
    cache_slots[side], x_delta >> ROAD_FIXED_FRACTION_BITS,
    y_delta >> ROAD_FIXED_FRACTION_BITS);
}

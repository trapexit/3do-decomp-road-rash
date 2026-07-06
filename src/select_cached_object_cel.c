#include "cel_dimension_resource.h"
#include "family_resource_runtime.h"
#include "road_geometry.h"

enum CachedObjectCelConstant
{
  CACHED_OBJECT_CEL_FAMILY_INDEX = 3
};

typedef struct RoadCelResourceDescriptor
{
  unsigned char prefix_bytes[0x39];
  unsigned char resource_selector;
} RoadCelResourceDescriptor;

typedef struct RoadCelObjectPrefix
{
  unsigned char prefix_bytes[0x0C];
  RoadCelResourceDescriptor *resource_descriptor;
} RoadCelObjectPrefix;

char *
select_cached_object_cel(char **cache_slot,
                         char  *road_object,
                         int    child_index,
                         int   *quad)
{
  const RoadCelObjectPrefix *typed_object;
  const RoadQuad *typed_quad;
  int x_delta;
  int y_delta;

  typed_object = (const RoadCelObjectPrefix *)(const void *)road_object;
  typed_quad = (const RoadQuad *)(const void *)quad;

  if(*cache_slot == 0)
    {
      *cache_slot = (char *)lookup_family_resource_child_for_render(
        typed_object->resource_descriptor->resource_selector,
        CACHED_OBJECT_CEL_FAMILY_INDEX, child_index);
      if(*cache_slot == 0)
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
    *cache_slot, x_delta >> ROAD_FIXED_FRACTION_BITS,
    y_delta >> ROAD_FIXED_FRACTION_BITS);
}

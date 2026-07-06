#include "cans_animation_binding.h"

enum { CANS_RESOURCE_LAYOUT_INDEXED = 1 };

typedef struct CansCompactResourceHeader
{
  int chunk_tag;
  int next_section_offset;
  int layout_type;
  int animation_name_count;
} CansCompactResourceHeader;

typedef struct CansIndexedResourceHeader
{
  int chunk_tag;
  int next_section_offset;
  int layout_type;
  int reserved[8];
  int animation_name_count;
} CansIndexedResourceHeader;

int
find_cans_animation_name(const AnimationResourcePair *resources,
                         int                          resource_key,
                         int                          start_index)
{
  const CansCompactResourceHeader *resource;
  const CansAnimationNameRecord *name;
  int name_count;
  int index;

  resource = (const CansCompactResourceHeader *)resources->cans_resource;
  if(resource->layout_type == CANS_RESOURCE_LAYOUT_INDEXED)
    {
      name_count =
        ((const CansIndexedResourceHeader *)resource)->animation_name_count;
    }
  else
    {
      name_count = resource->animation_name_count;
    }

  name = get_cans_animation_name_table(resources, start_index);
  index = start_index;
  while(name->resource_key != resource_key && index < name_count)
    {
      name++;
      index++;
    }
  if(index >= name_count)
    {
      index = -1;
    }
  return index;
}

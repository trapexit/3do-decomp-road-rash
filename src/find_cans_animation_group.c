#include "cans_animation_binding.h"

enum
{
  CANS_RESOURCE_LAYOUT_INDEXED = 1,
  CANS_RESOURCE_LAYOUT_FIRST_GROUP = 2,
  CANS_RESOURCE_LAYOUT_LAST_GROUP = 4
};

typedef struct CansCompactResourceHeader
{
  int chunk_tag;
  int next_section_offset;
  int layout_type;
  int animation_group_count;
} CansCompactResourceHeader;

typedef struct CansIndexedResourceHeader
{
  int chunk_tag;
  int next_section_offset;
  int layout_type;
  int reserved[8];
  int animation_group_count;
} CansIndexedResourceHeader;

const
CansAnimationGroup *
find_cans_animation_group(const AnimationResourcePair *resources,
                          int                          entry_index)
{
  const CansCompactResourceHeader *resource;
  const CansAnimationGroup *group;
  int group_count;
  int group_index;
  int layout_type;

  if(resources == 0 || resources->cans_resource == 0 ||
     entry_index < 0)
    {
      return 0;
    }

  resource = (const CansCompactResourceHeader *)resources->cans_resource;
  layout_type = resource->layout_type;
  if(layout_type == CANS_RESOURCE_LAYOUT_INDEXED)
    {
      group = (const CansAnimationGroup *)
              ((const CansIndexedResourceHeader *)resource + 1);
      group_count =
        ((const CansIndexedResourceHeader *)resource)
        ->animation_group_count;
    }
  else if(layout_type >= CANS_RESOURCE_LAYOUT_FIRST_GROUP &&
          layout_type <= CANS_RESOURCE_LAYOUT_LAST_GROUP)
    {
      group = (const CansAnimationGroup *)(resource + 1);
      group_count = resource->animation_group_count;
    }
  else
    {
      return 0;
    }

  for(group_index = 0; group_index < group_count; group_index++)
    {
      if(group->entry_number - 1 == entry_index)
        {
          return group;
        }
      if(group->variant_count < 0)
        {
          return 0;
        }
      group = (const CansAnimationGroup *)
              ((const char *)(group + 1) +
               group->variant_count *
               (int)sizeof(CansAnimationChannelRecord));
    }
  return 0;
}

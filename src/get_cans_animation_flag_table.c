#include "cans_animation_binding.h"

enum { CANS_RESOURCE_LAYOUT_INDEXED = 1 };

typedef struct CansLinkedSectionHeader
{
  int chunk_tag;
  int next_section_offset;
  int layout_type;
} CansLinkedSectionHeader;

const
int *
get_cans_animation_flag_table(const AnimationResourcePair *resources,
                              unsigned int                 entry_index)
{
  const CansLinkedSectionHeader *section;
  const int *flags;

  section = (const CansLinkedSectionHeader *)resources->cans_resource;
  if(section->layout_type != CANS_RESOURCE_LAYOUT_INDEXED)
    {
      section = (const CansLinkedSectionHeader *)
                ((const char *)section + section->next_section_offset);
    }
  section = (const CansLinkedSectionHeader *)
            ((const char *)section + section->next_section_offset);
  flags = (const int *)(section + 1);

  while(entry_index != 0)
    {
      entry_index--;
      flags += flags[0] + 1;
    }
  return flags;
}

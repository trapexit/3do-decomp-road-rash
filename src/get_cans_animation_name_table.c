#include "cans_animation_binding.h"

enum { CANS_NAME_TABLE_SECTION_HOPS = 4 };

typedef struct CansLinkedSectionHeader
{
  int chunk_tag;
  int next_section_offset;
  int layout_type;
} CansLinkedSectionHeader;

const
CansAnimationNameRecord *
get_cans_animation_name_table(const AnimationResourcePair *resources,
                              int                          entry_index)
{
  const CansLinkedSectionHeader *section;
  const CansAnimationNameRecord *names;
  int hop;

  section = (const CansLinkedSectionHeader *)resources->cans_resource;
  for(hop = 0; hop < CANS_NAME_TABLE_SECTION_HOPS; hop++)
    {
      section = (const CansLinkedSectionHeader *)
                ((const char *)section + section->next_section_offset);
    }
  names = (const CansAnimationNameRecord *)(section + 1);
  return &names[entry_index];
}

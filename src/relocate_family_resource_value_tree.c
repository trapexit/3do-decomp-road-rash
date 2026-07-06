#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"

#define FAMILY_RESOURCE_FORM_TAG          0x464F524DU
#define FAMILY_RESOURCE_CANS_TAG          0x43414E53U
#define FAMILY_RESOURCE_TAG_CLASS_MASK    0xF000U
#define FAMILY_RESOURCE_PLUT_DATA_OFFSET  12

typedef union FamilyResourceChildReference
{
  int relative_offset;
  void *relocated_value;
} FamilyResourceChildReference;

typedef struct FamilyResourceValueTree
{
  unsigned int child_count;
  FamilyResourceChildReference children[1];
} FamilyResourceValueTree;

void *
relocate_family_resource_value_tree(void *value)
{
  FamilyResourceValueTree *tree;
  unsigned int index;
  unsigned int tag;
  char *bytes;
  CCB *result;

  if(value == 0)
    {
      return 0;
    }

  bytes = (char *)value;
  tag = *(unsigned int *)bytes;
  if(tag == FAMILY_RESOURCE_FORM_TAG ||
     tag == CANS_CHUNK_TAG_ANIM ||
     tag == FAMILY_RESOURCE_CANS_TAG)
    {
      return value;
    }

  if(tag == CANS_CHUNK_TAG_CCB)
    {
      result = parse_cans_cel_chunk_stream(
        (CansChunkHeader *)bytes, 0);
      if(result == 0)
        {
          result = (CCB *)bytes;
        }
      return result;
    }

  if(tag == FAMILY_RESOURCE_CLGP_TAG)
    {
      return bytes + FAMILY_RESOURCE_CHUNK_HEADER_BYTES;
    }

  if(tag == CANS_CHUNK_TAG_OFSS)
    {
      CANS_RelocateAnimationOffsets((CansOffsetAnimation *)value);
      return value;
    }

  if(tag == CANS_CHUNK_TAG_OFST)
    {
      return bytes + *(int *)(bytes + sizeof(unsigned int));
    }

  if(tag == CANS_CHUNK_TAG_PLUT)
    {
      return bytes + FAMILY_RESOURCE_PLUT_DATA_OFFSET;
    }

  if((tag & FAMILY_RESOURCE_TAG_CLASS_MASK) != 0)
    {
      return value;
    }

  tree = (FamilyResourceValueTree *)value;
  for(index = 0; index < tree->child_count; index++)
    {
      tree->children[index].relocated_value =
        relocate_family_resource_value_tree(
          bytes + tree->children[index].relative_offset);
    }

  return bytes + sizeof(unsigned int);
}


#undef FAMILY_RESOURCE_FORM_TAG
#undef FAMILY_RESOURCE_CANS_TAG
#undef FAMILY_RESOURCE_TAG_CLASS_MASK
#undef FAMILY_RESOURCE_PLUT_DATA_OFFSET

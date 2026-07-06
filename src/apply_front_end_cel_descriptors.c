/* Reconstructed from the original function at 0x00008F28. */



#include "bss_early_data.h"
#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"

int
draw_front_end_cel_descriptor(const FrontEndCelDescriptor *descriptor);

int
apply_front_end_cel_descriptors(const FrontEndCelDescriptor *descriptors)
{
  signed char index;
  const FrontEndCelDescriptor *source;
  FrontEndCelHierarchyEntry *dst;

  index = 0;
  if(descriptors[0].resource_id == -1)
    {
      return -1;
    }

  do
    {
      source = &descriptors[index];
      dst = &gFrontEndCelHierarchy[source->resource_id];
      dst->resource_id = source->resource_id;
      dst->parent_index = source->parent_index;
      dst->child_selector =
        *(const unsigned int *)(const void *)&source->child_selector;
      draw_front_end_cel_descriptor(source);
      index = (signed char)(index + 1);
    } while(descriptors[index].resource_id != -1);

  return descriptors[index].resource_id;
}

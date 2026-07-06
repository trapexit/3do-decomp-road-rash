#include "rw_semantic_data.h"

int
draw_front_end_cel_descriptor(const FrontEndCelDescriptor *descriptor);

int
draw_front_end_cel(int resource_id)
{
  FrontEndCelDescriptor descriptor;
  unsigned int alignment_index;

  descriptor.resource_id = resource_id;
  descriptor.parent_index = -1;
  descriptor.child_selector = 0;
  for(alignment_index = 0;
      alignment_index < sizeof(descriptor.selector_alignment);
      alignment_index++)
    {
      descriptor.selector_alignment[alignment_index] = 0;
    }
  return draw_front_end_cel_descriptor(&descriptor);
}

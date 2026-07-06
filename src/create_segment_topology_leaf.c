#include "memory_manager_runtime.h"
#include "segment_topology_runtime.h"

SegmentTopologyNode *
create_segment_topology_leaf(SegmentResourceRecord *record)
{
  SegmentTopologyNode *node;

  node = MEM_NewPointer(SEGMENT_TOPOLOGY_NODE_BYTES, MEMORY_REQUEST_ANY,
                        MEM_POINTER_OPTIONS_DEFAULT);
  if(node != 0)
    {
      node->kind = SEGMENT_TOPOLOGY_LEAF;
      node->traversal_flags = 0;
      node->branches[0] = 0;
      node->branches[1] = 0;
      node->payload.record = record;
      node->endpoint_index = record->endpoint_index;
      node->selected_branch = 0;
      node->alternate_branch = 0;
      node->branch_alignment[0] = 0;
      node->branch_alignment[1] = 0;
      node->traversal_value = 0;
    }
  return node;
}

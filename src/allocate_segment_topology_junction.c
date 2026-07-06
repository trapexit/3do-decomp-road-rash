#include "memory_manager_runtime.h"
#include "segment_topology_runtime.h"

SegmentTopologyNode *
allocate_segment_topology_junction(SegmentTopologyKind kind)
{
  SegmentTopologyNode *node;

  node = MEM_NewPointer(SEGMENT_TOPOLOGY_NODE_BYTES, MEMORY_REQUEST_ANY,
                        MEM_POINTER_OPTIONS_DEFAULT);
  if(node != 0)
    {
      node->kind = kind;
      node->traversal_flags = 0;
      node->branches[0] = 0;
      node->branches[1] = 0;
      node->payload.source = 0;
      node->endpoint_index = SEGMENT_TOPOLOGY_UNSET_ENDPOINT;
      node->selected_branch = 0;
      node->alternate_branch = 1;
      node->branch_alignment[0] = 0;
      node->branch_alignment[1] = 0;
      node->traversal_value = 0;
    }
  return node;
}

#include "segment_topology_runtime.h"

SegmentTopologyNode *
relocate_segment_topology_offsets(SegmentTopologyContainer *container,
                                  int                       node_offset)
{
  SegmentTopologyNode *node;
  int flags;

  node = (SegmentTopologyNode *)((char *)container + node_offset);
  flags = node->traversal_flags;
  if((flags & SEGMENT_TOPOLOGY_POINTERS_RELOCATED) != 0)
    {
      return node;
    }

  node->traversal_flags =
    flags | SEGMENT_TOPOLOGY_POINTERS_RELOCATED;

  switch(node->kind)
    {
    case SEGMENT_TOPOLOGY_LEAF:
      node->branches[0] = relocate_segment_topology_offsets(
        container, (int)node->branches[0]);
      node->branches[1] = relocate_segment_topology_offsets(
        container, (int)node->branches[1]);
      node->selected_branch = 0;
      node->alternate_branch = 0;
      node->branch_alignment[0] = 0;
      node->branch_alignment[1] = 0;
      node->traversal_value = 0;
      break;

    case SEGMENT_TOPOLOGY_PRIMARY_JUNCTION:
    case SEGMENT_TOPOLOGY_SECONDARY_JUNCTION:
      node->payload.source = relocate_segment_topology_offsets(
        container, (int)node->payload.source);
      node->branches[0] = relocate_segment_topology_offsets(
        container, (int)node->branches[0]);
      node->branches[1] = relocate_segment_topology_offsets(
        container, (int)node->branches[1]);
      node->traversal_value = 0;
      break;
    }

  return node;
}

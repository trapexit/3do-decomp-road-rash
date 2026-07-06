#include "segment_topology_runtime.h"

void
relocate_and_repair_segment_topology(SegmentTopologyNode       *node,
                                     SegmentResourceCollection *resources)
{
  int value;

  value = node->traversal_flags;
  if((value & SEGMENT_TOPOLOGY_FINALIZED) != 0)
    {
      return;
    }
  node->traversal_flags = value | SEGMENT_TOPOLOGY_FINALIZED;

  switch(node->kind)
    {
    case SEGMENT_TOPOLOGY_LEAF:
      node->payload.record = resources->records +
                             (int)node->payload.record;
      normalize_segment_leaf_resource(
        node, (int)node->payload.record);
      relocate_and_repair_segment_topology(node->branches[0], resources);
      relocate_and_repair_segment_topology(node->branches[1], resources);
      return;

    case SEGMENT_TOPOLOGY_PRIMARY_JUNCTION:
    case SEGMENT_TOPOLOGY_SECONDARY_JUNCTION:
      relocate_and_repair_segment_topology(node->payload.source, resources);
      relocate_and_repair_segment_topology(node->branches[0], resources);
      relocate_and_repair_segment_topology(node->branches[1], resources);
      reconcile_segment_junction_boundaries(node);
      return;
    }
}

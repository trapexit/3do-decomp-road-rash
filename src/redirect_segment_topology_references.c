#include "segment_topology_runtime.h"

void
redirect_segment_topology_references(SegmentTopologyNode *node,
                                     SegmentTopologyNode *replacement)
{
  SegmentTopologyNode *predecessor;
  SegmentTopologyNode *successor;

  if((node->traversal_flags &
      SEGMENT_TOPOLOGY_REFERENCE_VISITED) != 0)
    {
      return;
    }

  node->traversal_flags |= SEGMENT_TOPOLOGY_REFERENCE_VISITED;
  predecessor = node;
  for(;;)
    {
      successor = predecessor->branches[SEGMENT_BRANCH_PRIMARY];
      if(successor->kind != SEGMENT_TOPOLOGY_LEAF ||
         successor == node)
        {
          break;
        }
      predecessor = successor;
    }

  if(predecessor->branches[SEGMENT_BRANCH_PRIMARY] == node)
    {
      return;
    }

  if(successor->kind == SEGMENT_TOPOLOGY_PRIMARY_JUNCTION)
    {
      redirect_segment_topology_references(
        successor->branches[SEGMENT_BRANCH_PRIMARY], replacement);
      redirect_segment_topology_references(
        successor->branches[SEGMENT_BRANCH_SECONDARY], replacement);
      return;
    }

  if(successor->kind == SEGMENT_TOPOLOGY_FORWARD_REFERENCE ||
     successor->kind == SEGMENT_TOPOLOGY_REVERSE_REFERENCE)
    {
      predecessor->branches[SEGMENT_BRANCH_PRIMARY] = replacement;
    }
}

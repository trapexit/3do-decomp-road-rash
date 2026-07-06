#include "segment_topology_runtime.h"

SegmentTopologyNode *
find_segment_branch_reference_parent(SegmentTopologyNode  *node,
                                     SegmentBranchSelector selector)
{
  SegmentTopologyNode *successor;
  int branch_index;

  successor = node->branches[SEGMENT_BRANCH_PRIMARY];
  if(successor->kind >= SEGMENT_TOPOLOGY_FORWARD_REFERENCE)
    {
      return node;
    }

  do
    {
      if(successor->kind == SEGMENT_TOPOLOGY_LEAF)
        {
          node = successor;
        }
      else if(successor->kind ==
              SEGMENT_TOPOLOGY_PRIMARY_JUNCTION)
        {
          if(selector == SEGMENT_BRANCH_SELECTED)
            {
              branch_index = successor->selected_branch;
            }
          else if(selector == SEGMENT_BRANCH_ALTERNATE)
            {
              branch_index = successor->alternate_branch;
            }
          else
            {
              branch_index = selector;
            }
          if(branch_index > SEGMENT_BRANCH_SECONDARY)
            {
              branch_index = SEGMENT_BRANCH_SECONDARY;
            }
          node = successor->branches[branch_index];
        }
      else
        {
          node = successor->payload.source;
        }

      successor = node->branches[SEGMENT_BRANCH_PRIMARY];
    } while(successor->kind < SEGMENT_TOPOLOGY_FORWARD_REFERENCE);

  return node;
}

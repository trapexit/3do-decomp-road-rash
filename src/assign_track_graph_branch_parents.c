#include "track_segment_definition.h"

void
assign_track_graph_branch_parents(RoadTrackGraphNode *node,
                                  RoadTrackGraphNode *parent,
                                  int                 secondary_branch)
{
  unsigned int branch_flag;
  unsigned int flags;

  flags = node->flags;
  if((flags & ROAD_TRACK_GRAPH_NODE_BRANCH_MASK) != 0)
    {
      return;
    }

  if(parent == 0)
    {
      branch_flag = ROAD_TRACK_GRAPH_NODE_BRANCH_MASK;
    }
  else if(secondary_branch != 0)
    {
      branch_flag = ROAD_TRACK_GRAPH_NODE_SECONDARY_BRANCH;
    }
  else
    {
      branch_flag = ROAD_TRACK_GRAPH_NODE_PRIMARY_BRANCH;
    }

  while(node->type == ROAD_TRACK_GRAPH_NODE_SEGMENT)
    {
      node->flags |= branch_flag;
      node->payload.segment_definition->parent_junction = parent;
      node = node->primary_link;
    }

  if(node->type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
    {
      assign_track_graph_branch_parents(node->primary_link, node, 0);
      assign_track_graph_branch_parents(node->secondary_link, node, 1);
    }
  else if(node->type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
    {
      assign_track_graph_branch_parents(node->payload.single_link, 0, 0);
    }
}

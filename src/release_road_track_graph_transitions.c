#include "track_segment_definition.h"

int
release_road_track_graph_transitions(RoadTrackGraphNode *root)
{
  RoadTrackGraphNode *node;
  int type;
  unsigned int flags;

  if(root == 0)
    {
      return 0;
    }

  flags = root->flags;
  if((flags & ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED) != 0)
    {
      return (int)flags;
    }

  node = root;
  while(node->type == ROAD_TRACK_GRAPH_NODE_SEGMENT)
    {
      node->flags |= ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED;
      node = node->primary_link;
    }

  type = node->type;
  if((type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH ||
      type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH) &&
     (node->flags & ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED) == 0)
    {
      node->flags |= ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED;
      release_road_track_transition_runtime(node);
      release_road_track_graph_transitions(node->payload.single_link);
      release_road_track_graph_transitions(node->primary_link);
      release_road_track_graph_transitions(node->secondary_link);
    }

  while(root->type == ROAD_TRACK_GRAPH_NODE_SEGMENT)
    {
      root->flags |= ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED;
      root = root->secondary_link;
    }

  type = root->type;
  if(type != ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH &&
     type != ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
    {
      return type;
    }

  flags = root->flags;
  if((flags & ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED) != 0)
    {
      return (int)flags;
    }

  root->flags = flags | ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED;
  release_road_track_transition_runtime(root);
  release_road_track_graph_transitions(root->payload.single_link);
  release_road_track_graph_transitions(root->primary_link);
  return release_road_track_graph_transitions(root->secondary_link);
}

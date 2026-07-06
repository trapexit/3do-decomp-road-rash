#include "track_segment_definition.h"

void
prepare_track_hazard_table(TrackHazardTable *table);

int
prepare_track_resource_graph(RoadTrackGraphNode *node)
{
  TrackSegmentDefinition *definition;
  unsigned int flags;
  int result;

  if(node == 0)
    {
      return 1;
    }

  flags = node->flags;
  if((flags & ROAD_TRACK_GRAPH_NODE_VISITED) != 0)
    {
      return 1;
    }
  node->flags = flags | ROAD_TRACK_GRAPH_NODE_VISITED;

  switch(node->type)
    {
    case ROAD_TRACK_GRAPH_NODE_SEGMENT:
      definition = node->payload.segment_definition;
      prepare_track_hazard_table(definition->hazard_table);
      result = find_active_level_finish_sample(
        definition->section_resource);
      if(result != 0)
        {
          definition->finish_sample_offset = result;
        }
      if(prepare_track_resource_graph(node->primary_link) == 0)
        {
          return 0;
        }
      break;

    case ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH:
      node->extent.transition_span = ROAD_TRACK_TRANSITION_SCAN_SPAN;
      fix_road_junction_elevations(node);
      if(create_road_track_transition_runtime(
           node, ROAD_TRACK_TRANSITION_SAMPLE_FORWARD) == 0)
        {
          return 0;
        }
      if(prepare_track_resource_graph(node->primary_link) == 0)
        {
          return 0;
        }
      if(prepare_track_resource_graph(node->secondary_link) == 0)
        {
          return 0;
        }
      break;

    case ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH:
      node->extent.transition_span = ROAD_TRACK_TRANSITION_SCAN_SPAN;
      fix_road_junction_elevations(node);
      if(create_road_track_transition_runtime(
           node, ROAD_TRACK_TRANSITION_SAMPLE_REVERSE) == 0)
        {
          return 0;
        }
      if(prepare_track_resource_graph(node->payload.single_link) == 0)
        {
          return 0;
        }
      break;
    }

  return 1;
}

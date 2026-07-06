#include "rw_semantic_data.h"
#include "track_segment_definition.h"

int
calculate_track_finish_distances(RoadTrackGraphNode *node,
                                 int                 distance_from_finish)
{
  TrackSegmentDefinition *definition;
  RoadTrackGraphNode *linked_node;
  unsigned int flags;
  int finish_distance;

  finish_distance = 0;
  if(distance_from_finish < 0)
    {
      distance_from_finish = 0;
      track_runtime_globals.traversal_root = node;
    }

  switch(node->type)
    {
    case ROAD_TRACK_GRAPH_NODE_SEGMENT:
      flags = node->flags;
      if((flags & ROAD_TRACK_GRAPH_NODE_FINISH_DISTANCE_SET) != 0)
        {
          if(node == track_runtime_globals.traversal_root)
            {
              return finish_distance;
            }
          return node->payload.segment_definition->finish_distance;
        }

      node->flags = flags | ROAD_TRACK_GRAPH_NODE_FINISH_DISTANCE_SET;
      definition = node->payload.segment_definition;
      if(distance_from_finish > 0)
        {
          finish_distance = -distance_from_finish;
          calculate_track_finish_distances(
            node->primary_link,
            distance_from_finish + node->extent.segment_sample_count);
        }
      else if(definition->finish_sample_offset != 0)
        {
          finish_distance = definition->finish_sample_offset;
          calculate_track_finish_distances(
            node->primary_link,
            node->extent.segment_sample_count - finish_distance);
        }
      else
        {
          finish_distance = calculate_track_finish_distances(
            node->primary_link, 0) + node->extent.segment_sample_count;
        }

      definition->finish_distance = finish_distance;
      return finish_distance;

    case ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH:
      linked_node = (&node->primary_link)[node->main_fork];
      finish_distance = calculate_track_finish_distances(
        linked_node, distance_from_finish);
      linked_node = (&node->primary_link)[node->alternate_fork];
      calculate_track_finish_distances(linked_node,
                                       distance_from_finish);
      return finish_distance;

    case ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH:
      return calculate_track_finish_distances(
        node->payload.single_link, distance_from_finish);
    }

  return finish_distance;
}

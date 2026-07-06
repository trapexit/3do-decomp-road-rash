#include "stdio.h"
#include "track_segment_definition.h"

/* Reconstructed from the original function at 0x00022D9C. */

int
fix_road_junction_elevations(RoadTrackGraphNode *junction)
{
  RoadTrackGraphNode *source_node;
  RoadTrackGraphNode *destination_node;
  TrackSegmentDefinition *source_definition;
  TrackSegmentDefinition *destination_definition;
  int fork_index;
  int sample_count;
  int sample_index;
  const RoadPathSample *source_samples;
  RoadPathSample *destination_samples;

  fork_index = junction->main_fork;
  if((unsigned int)fork_index >= ROAD_TRACK_CHANNEL_COUNT)
    {
      printf("###fixJunctionElevations: bad mainFork field in junction\n");
      fork_index = ROAD_TRACK_SECOND_CHANNEL;
    }

  if(fork_index == ROAD_TRACK_FIRST_CHANNEL)
    {
      source_node = junction->primary_link;
      destination_node = junction->secondary_link;
    }
  else
    {
      source_node = junction->secondary_link;
      destination_node = junction->primary_link;
    }

  source_definition = source_node->payload.segment_definition;
  destination_definition = destination_node->payload.segment_definition;
  source_samples = source_definition->path_resource->samples;
  destination_samples = destination_definition->path_resource->samples;

  sample_count =
    junction->extent.transition_span >> TRACK_POSITION_FRACTION_BITS;
  if(junction->type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
    {
      source_samples +=
        source_node->extent.segment_sample_count - sample_count;
      destination_samples +=
        destination_node->extent.segment_sample_count - sample_count;
    }

  if(sample_count <= 0)
    {
      return sample_count;
    }

  for(sample_index = 0; sample_index < sample_count; sample_index++)
    {
      destination_samples[sample_index][ROAD_PATH_ELEVATION_STEP_INDEX] =
        source_samples[sample_index][ROAD_PATH_ELEVATION_STEP_INDEX];
    }

  return sample_count;
}

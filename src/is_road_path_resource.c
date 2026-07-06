#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x00018B30. */

int
is_road_path_resource(const RoadPathResource *resource)
{
  return (resource->tag == ROAD_PATH_RESOURCE_TAG) &
         TRACK_DIRECTION_BYTE_MASK;
}


int
get_road_path_sample_count(const RoadPathResource *resource)
{
  if(is_road_path_resource(resource) == 0)
    {
      return ROAD_PATH_DEFAULT_SAMPLE_COUNT;
    }
  return resource->sample_count;
}

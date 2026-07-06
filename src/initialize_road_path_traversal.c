#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001FD48. */

#define ROAD_PATH_READ_SIGNED_BE16(bytes) \
        ((signed short)((((unsigned int)(bytes)[0]) << 8) | (bytes)[1]))

void
initialize_road_path_traversal(RoadPathTraversalState *path_state,
                               const RoadPathResource *resource,
                               int                     forward)
{
  forward &= TRACK_DIRECTION_BYTE_MASK;
  path_state->resource = resource;
  if(resource == 0)
    {
      return;
    }

  if(forward != 0)
    {
      path_state->elevation = ROAD_PATH_READ_SIGNED_BE16(
        (const unsigned char *)&resource->start_elevation);
      path_state->track_position = 0;
      return;
    }

  path_state->elevation = ROAD_PATH_READ_SIGNED_BE16(
    (const unsigned char *)&resource->end_elevation);
  path_state->track_position =
    resource->sample_count << TRACK_POSITION_FRACTION_BITS;
}


#undef ROAD_PATH_READ_SIGNED_BE16

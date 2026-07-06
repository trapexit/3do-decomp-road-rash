#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001FE00. */

void
advance_road_path_traversal(RoadPathTraversalState *path_state,
                            int                     delta)
{
  int old_position;
  int new_position;
  int sample_index;

  old_position = path_state->track_position;
  new_position = old_position + delta;
  path_state->track_position = new_position;

  if((old_position >> TRACK_POSITION_FRACTION_BITS) ==
     (new_position >> TRACK_POSITION_FRACTION_BITS))
    {
      return;
    }

  if(delta > 0)
    {
      sample_index = old_position >> TRACK_POSITION_FRACTION_BITS;
      path_state->elevation +=
        path_state->resource->samples[sample_index]
        [ROAD_PATH_ELEVATION_STEP_INDEX];
      return;
    }

  sample_index = new_position >> TRACK_POSITION_FRACTION_BITS;
  path_state->elevation -=
    path_state->resource->samples[sample_index]
    [ROAD_PATH_ELEVATION_STEP_INDEX];
}

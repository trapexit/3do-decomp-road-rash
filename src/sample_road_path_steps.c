#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001FE6C. */

int
sample_road_path_steps(const RoadPathTraversalState *path_state,
                       int                          *out_curvature_step,
                       int                          *out_elevation_step)
{
  const RoadPathSample *sample;
  int sample_index;

  if(path_state->resource == 0)
    {
      *out_curvature_step = 0;
      *out_elevation_step = 0;
      return 0;
    }

  sample_index =
    path_state->track_position >> TRACK_POSITION_FRACTION_BITS;
  sample = &path_state->resource->samples[sample_index];
  *out_curvature_step = (*sample)[ROAD_PATH_CURVATURE_STEP_INDEX];
  *out_elevation_step = (*sample)[ROAD_PATH_ELEVATION_STEP_INDEX];
  return *out_elevation_step;
}

#include "bss_late_data.h"
#include "road_geometry.h"

/* Reconstructed from the original function at 0x0001FB64. */

void
populate_road_terrain_surface_resources(const RoadTerrainTraversalState *terrain_state,
                                        RoadSegmentLaneRuntime          *lane_runtime)
{
  const RoadTerrainSurfaceSample *surface_sample;

  surface_sample = &terrain_state->surface_resource_samples[
    terrain_state->track_position >> TRACK_POSITION_FRACTION_BITS];
  lane_runtime->surface_resource_ids[ROAD_SIDE_LEFT] =
    (*surface_sample)[ROAD_SIDE_LEFT];
  lane_runtime->surface_resource_ids[ROAD_SIDE_RIGHT] =
    (*surface_sample)[ROAD_SIDE_RIGHT];
  lane_runtime->geometry_mode = ROAD_GEOMETRY_MODE_TEXTURED;
}

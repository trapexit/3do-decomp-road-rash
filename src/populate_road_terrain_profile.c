#include "bss_late_data.h"
#include "road_geometry.h"

/* Reconstructed from the original function at 0x0001FCC4. */

int
populate_road_terrain_profile(const RoadTerrainTraversalState *terrain_state,
                              RoadSegmentLaneRuntime          *lane_runtime)
{
  if(lane_runtime != 0)
    {
      lane_runtime->geometry_mode = ROAD_GEOMETRY_MODE_UNSET;
    }
  if(terrain_state == 0)
    {
      return 0;
    }

  if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RBLD)
    {
      populate_road_terrain_surface_resources(terrain_state, lane_runtime);
    }
  else if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RDWD)
    {
      populate_road_terrain_edge_shapes(terrain_state, lane_runtime);
    }
  else if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RHIL)
    {
      populate_hill_road_terrain_profile(terrain_state, lane_runtime);
    }
  else if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RMTN)
    {
      populate_road_terrain_edge_shapes(terrain_state, lane_runtime);
    }

  return 0;
}

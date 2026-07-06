#include "bss_late_data.h"

/* Reconstructed from the original function at 0x0001FA6C. */

void
advance_road_terrain_traversal(RoadTerrainTraversalState *terrain_state,
                               int                        delta)
{
  if(terrain_state == 0)
    {
      return;
    }

  terrain_state->track_position += delta;
  if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RBLD)
    {
      return;
    }

  if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RDWD ||
     terrain_state->resource_tag == ROAD_TERRAIN_TAG_RMTN)
    {
      advance_road_terrain_edge_traversal(terrain_state, delta);
    }
  else if(terrain_state->resource_tag == ROAD_TERRAIN_TAG_RHIL)
    {
      advance_hill_road_terrain_traversal(terrain_state, delta);
    }
}

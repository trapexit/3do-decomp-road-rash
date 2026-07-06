#include "bss_late_data.h"
#include "road_geometry.h"

/* Reconstructed from the original function at 0x0001FAB8. */

enum RoadTerrainEdgeShapeScale
{
  ROAD_TERRAIN_EDGE_INNER_MIDPOINT_SHIFT = 1,
  ROAD_TERRAIN_EDGE_SLOPE_WIDTH_SHIFT = 2
};

void
populate_road_terrain_edge_shapes(const RoadTerrainTraversalState *terrain_state,
                                  RoadSegmentLaneRuntime          *lane_runtime)
{
  const RoadTerrainEdgeVariantState *edge_state;
  const RoadTerrainEdgeEntry *entry;
  const RoadTerrainSurfaceSample *surface_sample;
  int side;

  if(lane_runtime == 0)
    {
      return;
    }

  surface_sample = &terrain_state->surface_resource_samples[
    terrain_state->track_position >> TRACK_POSITION_FRACTION_BITS];
  lane_runtime->surface_resource_ids[ROAD_SIDE_LEFT] =
    (*surface_sample)[ROAD_TERRAIN_SIDE_LEFT];
  lane_runtime->surface_resource_ids[ROAD_SIDE_RIGHT] =
    (*surface_sample)[ROAD_TERRAIN_SIDE_RIGHT];

  edge_state = &terrain_state->variant.edge;
  for(side = ROAD_TERRAIN_SIDE_LEFT;
      side < ROAD_TERRAIN_SIDE_COUNT; side++)
    {
      entry = edge_state->active_entries[side];
      lane_runtime->edge_shapes[side].inner_offset =
        edge_state->inner_offsets[side];
      lane_runtime->edge_shapes[side].outer_offset =
        ((int)entry->inner_midpoint <<
          ROAD_TERRAIN_EDGE_INNER_MIDPOINT_SHIFT) +
        ((int)entry->slope_width_raw <<
          ROAD_TERRAIN_EDGE_SLOPE_WIDTH_SHIFT) +
        edge_state->outer_adjustments[side];
      lane_runtime->edge_shapes[side].height =
        edge_state->height_adjustments[side] + entry->base_height;
    }

  lane_runtime->geometry_mode = ROAD_GEOMETRY_MODE_EDGE;
}

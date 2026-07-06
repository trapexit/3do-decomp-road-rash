#include "bss_late_data.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x0001F2B4. */

enum RoadTerrainInitializationScale
{
  ROAD_TERRAIN_HEIGHT_INITIAL_SHIFT = 1,
  ROAD_TERRAIN_OUTER_INITIAL_SHIFT = 1
};

static
void
initialize_road_terrain_edge_variant(RoadTerrainTraversalState     *terrain_state,
                                     const RoadTerrainEdgeResource *resource,
                                     int                            initial_position)
{
  RoadTerrainEdgeVariantState *edge_state;
  const RoadTerrainEdgeEntry *entry;
  int side;
  unsigned int left_entry_count;
  unsigned int total_entry_count;

  edge_state = &terrain_state->variant.edge;
  left_entry_count = resource->entry_counts[ROAD_TERRAIN_SIDE_LEFT];
  total_entry_count =
    left_entry_count +
    resource->entry_counts[ROAD_TERRAIN_SIDE_RIGHT];

  if(initial_position == 0)
    {
      edge_state->active_entry_indices[ROAD_TERRAIN_SIDE_LEFT] = 0;
      edge_state->active_entry_indices[ROAD_TERRAIN_SIDE_RIGHT] = 0;
    }
  else
    {
      edge_state->active_entry_indices[ROAD_TERRAIN_SIDE_LEFT] =
        left_entry_count - 1;
      edge_state->active_entry_indices[ROAD_TERRAIN_SIDE_RIGHT] =
        resource->entry_counts[ROAD_TERRAIN_SIDE_RIGHT] - 1;
    }

  edge_state->active_entries[ROAD_TERRAIN_SIDE_LEFT] =
    &resource->entries[
      edge_state->active_entry_indices[ROAD_TERRAIN_SIDE_LEFT]];
  edge_state->active_entries[ROAD_TERRAIN_SIDE_RIGHT] =
    &resource->entries[
      left_entry_count +
      edge_state->active_entry_indices[ROAD_TERRAIN_SIDE_RIGHT]];
  terrain_state->surface_resource_samples =
    (const RoadTerrainSurfaceSample *)
    &resource->entries[total_entry_count];

  for(side = ROAD_TERRAIN_SIDE_LEFT;
      side < ROAD_TERRAIN_SIDE_COUNT; side++)
    {
      entry = edge_state->active_entries[side];
      edge_state->inner_offsets[side] = entry->inner_midpoint;
      edge_state->height_adjustments[side] =
        (int)entry->height_midpoint_raw <<
          ROAD_TERRAIN_HEIGHT_INITIAL_SHIFT;
      edge_state->outer_adjustments[side] =
        (int)entry->outer_midpoint_raw <<
          ROAD_TERRAIN_OUTER_INITIAL_SHIFT;
    }
}


void
initialize_road_terrain_traversal(RoadTerrainTraversalState       *terrain_state,
                                  const RoadTerrainResourceHeader *resource,
                                  int                              initial_position)
{
  const RoadTerrainSurfaceResource *surface_resource;

  terrain_state->resource = resource;
  if(resource == 0)
    {
      terrain_state->resource_tag = (unsigned int)-1;
      return;
    }

  terrain_state->resource_tag = resource->tag;
  if(resource->tag == ROAD_TERRAIN_TAG_RBLD)
    {
      surface_resource = (const RoadTerrainSurfaceResource *)resource;
      terrain_state->surface_resource_samples =
        &surface_resource->samples[0];
      terrain_state->variant.edge.active_entries[
        ROAD_TERRAIN_SIDE_LEFT] = 0;
      terrain_state->variant.edge.active_entries[
        ROAD_TERRAIN_SIDE_RIGHT] = 0;
    }
  else if(resource->tag == ROAD_TERRAIN_TAG_RDWD ||
          resource->tag == ROAD_TERRAIN_TAG_RMTN)
    {
      initialize_road_terrain_edge_variant(
        terrain_state, (const RoadTerrainEdgeResource *)resource,
        initial_position);
    }
  else if(resource->tag == ROAD_TERRAIN_TAG_RHIL)
    {
      initialize_hill_road_terrain_variant_state(
        terrain_state, (const RoadHillTerrainResource *)resource,
        initial_position);
    }

  terrain_state->track_position = initial_position;
  terrain_state->sample_index =
    initial_position >> TRACK_POSITION_FRACTION_BITS;
}

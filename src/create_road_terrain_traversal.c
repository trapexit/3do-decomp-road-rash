#include "bss_late_data.h"
#include "division_helpers.h"
#include "memory_manager_runtime.h"
#include "stdlib.h"

/* Reconstructed from the original function at 0x0001F41C. */

enum RoadTerrainEdgeTraversalScale
{
  ROAD_TERRAIN_RANDOM_DIAMETER_MULTIPLIER = 2,
  ROAD_TERRAIN_INNER_LIMIT_SHIFT = 1,
  ROAD_TERRAIN_HEIGHT_LIMIT_SHIFT = 2,
  ROAD_TERRAIN_OUTER_LIMIT_SHIFT = 2
};

RoadTerrainTraversalState *
create_road_terrain_traversal(RoadTerrainTraversalState       *terrain_state,
                              const RoadTerrainResourceHeader *resource)
{
  if(terrain_state == 0)
    {
      terrain_state = (RoadTerrainTraversalState *)MEM_NewPointer(
        sizeof(*terrain_state), MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(terrain_state != 0)
    {
      initialize_road_terrain_traversal(terrain_state, resource, 0);
    }
  return terrain_state;
}


static
int
choose_centered_road_terrain_adjustment(int radius)
{
  UnsignedDivisionResult division_result;
  unsigned int value_count;

  value_count =
    (unsigned int)(radius *
                   ROAD_TERRAIN_RANDOM_DIAMETER_MULTIPLIER + 1);
  division_result = unsigned_divide_with_remainder(
    value_count, (unsigned int)rand());
  return (int)division_result.remainder - radius;
}


static
int
clamp_road_terrain_adjustment(int value,
                              int maximum)
{
  if(value < 0)
    {
      return 0;
    }
  if(value > maximum)
    {
      return maximum;
    }
  return value;
}


void
advance_road_terrain_edge_traversal(RoadTerrainTraversalState *terrain_state,
                                    int                        delta)
{
  RoadTerrainEdgeVariantState *edge_state;
  const RoadTerrainEdgeResource *resource;
  const RoadTerrainEdgeEntry **active_entries;
  const RoadTerrainEdgeEntry *entry;
  int target_sample;
  int side;
  int value;
  int maximum;

  edge_state = &terrain_state->variant.edge;
  active_entries = edge_state->active_entries;
  if(active_entries == 0)
    {
      return;
    }

  target_sample =
    terrain_state->track_position >> TRACK_POSITION_FRACTION_BITS;
  if(target_sample != terrain_state->sample_index)
    {
      resource = (const RoadTerrainEdgeResource *)
                 terrain_state->resource;
      for(side = ROAD_TERRAIN_SIDE_LEFT;
          side < ROAD_TERRAIN_SIDE_COUNT; side++)
        {
          entry = active_entries[side];

          if(resource->entry_counts[side] > 1)
            {
              if(delta > 0)
                {
                  if(entry[1].start_sample <= target_sample)
                    {
                      edge_state->active_entry_indices[side] += 1;
                      entry += 1;
                      active_entries[side] = entry;
                    }
                }
              else if(entry->start_sample > target_sample)
                {
                  edge_state->active_entry_indices[side] -= 1;
                  entry -= 1;
                  active_entries[side] = entry;
                }
            }

          /*
           * The oracle consumes both draws but leaves the inner and height
           * traversal values unchanged.
           */
          (void)choose_centered_road_terrain_adjustment(
            entry->inner_step_radius);
          (void)choose_centered_road_terrain_adjustment(
            entry->height_step_radius);

          edge_state->outer_adjustments[side] +=
            choose_centered_road_terrain_adjustment(
              entry->outer_step_radius);

          value = edge_state->inner_offsets[side];
          maximum =
            (int)entry->inner_midpoint <<
              ROAD_TERRAIN_INNER_LIMIT_SHIFT;
          edge_state->inner_offsets[side] =
            clamp_road_terrain_adjustment(value, maximum);

          value = edge_state->height_adjustments[side];
          maximum =
            (int)entry->height_midpoint_raw <<
              ROAD_TERRAIN_HEIGHT_LIMIT_SHIFT;
          edge_state->height_adjustments[side] =
            clamp_road_terrain_adjustment(value, maximum);

          value = edge_state->outer_adjustments[side];
          maximum =
            (int)entry->outer_midpoint_raw <<
              ROAD_TERRAIN_OUTER_LIMIT_SHIFT;
          edge_state->outer_adjustments[side] =
            clamp_road_terrain_adjustment(value, maximum);
        }
    }

  terrain_state->sample_index = target_sample;
}

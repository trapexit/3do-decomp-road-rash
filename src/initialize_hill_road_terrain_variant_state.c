#include "bss_late_data.h"
#include "division_helpers.h"
#include "stdlib.h"

/* Reconstructed from the original function at 0x0001F188. */

enum RoadHillTerrainInitialValueScale
{
  ROAD_HILL_INITIAL_X_SCALE = 32,
  ROAD_HILL_INITIAL_Y_SCALE = 16
};

static
int
choose_hill_terrain_initial_value(int minimum,
                                  int maximum)
{
  UnsignedDivisionResult division_result;
  unsigned int value_count;

  value_count = (unsigned int)(maximum - minimum + 1);
  division_result = unsigned_divide_with_remainder(
    value_count, (unsigned int)rand());
  return (int)division_result.remainder + minimum;
}


void
initialize_hill_road_terrain_variant_state(RoadTerrainTraversalState     *terrain_state,
                                           const RoadHillTerrainResource *resource,
                                           int                            initial_position)
{
  RoadHillTerrainVariantState *hill_state;
  const RoadHillTerrainDefinition *definition;
  const RoadHillTerrainControl *control;
  int side;
  int control_index;
  int minimum;
  int maximum;

  hill_state = &terrain_state->variant.hill;
  terrain_state->surface_resource_samples = 0;
  hill_state->unused_shape_entries[0] = 0;
  hill_state->unused_shape_entries[1] = 0;

  if(initial_position != 0)
    {
      hill_state->definition_index = resource->definition_count - 1;
    }
  else
    {
      hill_state->definition_index = 0;
    }
  definition =
    &resource->definitions[hill_state->definition_index];
  hill_state->current_definition = definition;

  for(side = 0; side < ROAD_TERRAIN_SIDE_COUNT; side++)
    {
      for(control_index = 0;
          control_index < ROAD_HILL_TERRAIN_CONTROL_COUNT;
          control_index++)
        {
          hill_state->interval_starts[side][control_index] =
            initial_position;
          hill_state->interval_ends[side][control_index] =
            initial_position;

          control = &definition->controls[side][control_index];
          minimum =
            (int)control->minimum_x * ROAD_HILL_INITIAL_X_SCALE;
          maximum =
            (int)control->maximum_x * ROAD_HILL_INITIAL_X_SCALE;
          hill_state->current_points[side][control_index].x =
            choose_hill_terrain_initial_value(minimum, maximum);

          minimum =
            (int)control->minimum_y * ROAD_HILL_INITIAL_Y_SCALE;
          maximum =
            (int)control->maximum_y * ROAD_HILL_INITIAL_Y_SCALE;
          hill_state->current_points[side][control_index].y =
            choose_hill_terrain_initial_value(minimum, maximum);
        }
    }
}

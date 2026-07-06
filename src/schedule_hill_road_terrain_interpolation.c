#include "bss_late_data.h"
#include "division_helpers.h"
#include "stdlib.h"

/* Reconstructed from the original function at 0x0001F67C. */

enum RoadHillTerrainInterpolationScale
{
  ROAD_HILL_RANDOM_DIAMETER_MULTIPLIER = 2,
  ROAD_HILL_X_BOUND_SCALE = 32,
  ROAD_HILL_Y_BOUND_SCALE = 16,
  ROAD_HILL_DELTA_RADIUS_SCALE = 8,
  ROAD_HILL_FORWARD_SAMPLE_STEP = 1,
  ROAD_HILL_REVERSE_SAMPLE_STEP = -1
};

static
int
choose_centered_hill_terrain_delta(int radius)
{
  UnsignedDivisionResult division_result;
  unsigned int value_count;

  value_count =
    (unsigned int)(radius *
                   ROAD_HILL_RANDOM_DIAMETER_MULTIPLIER + 1);
  division_result = unsigned_divide_with_remainder(
    value_count, (unsigned int)rand());
  return (int)division_result.remainder - radius;
}


static
int
divide_signed_hill_terrain_delta(int divisor,
                                 int numerator)
{
  SignedDivisionResult division_result;

  division_result = signed_divide_with_remainder(
    (unsigned int)divisor, (unsigned int)numerator);
  return division_result.quotient;
}


static
void
choose_hill_terrain_interpolation_step(RoadHillTerrainPoint         *interpolation_step,
                                       const RoadHillTerrainPoint   *current_point,
                                       const RoadHillTerrainControl *control,
                                       int                           run)
{
  int radius;
  int delta;
  int minimum;
  int maximum;
  int target;

  radius =
    (int)control->x_step_radius * ROAD_HILL_DELTA_RADIUS_SCALE;
  delta = choose_centered_hill_terrain_delta(radius);
  minimum = (int)control->minimum_x * ROAD_HILL_X_BOUND_SCALE;
  maximum = (int)control->maximum_x * ROAD_HILL_X_BOUND_SCALE;
  target = current_point->x + run * delta;
  if(target < minimum)
    {
      delta = divide_signed_hill_terrain_delta(
        run, minimum - current_point->x);
    }
  else if(target > maximum)
    {
      delta = divide_signed_hill_terrain_delta(
        run, maximum - current_point->x);
    }
  interpolation_step->x = delta;

  radius =
    (int)control->y_step_radius * ROAD_HILL_DELTA_RADIUS_SCALE;
  delta = choose_centered_hill_terrain_delta(radius);
  minimum = (int)control->minimum_y * ROAD_HILL_Y_BOUND_SCALE;
  maximum = (int)control->maximum_y * ROAD_HILL_Y_BOUND_SCALE;
  target = current_point->y + run * delta;
  if(target < minimum)
    {
      delta = divide_signed_hill_terrain_delta(
        run, minimum - current_point->y);
    }
  else if(target > maximum)
    {
      delta = divide_signed_hill_terrain_delta(
        run, maximum - current_point->y);
    }
  interpolation_step->y = delta;
}


void
schedule_hill_road_terrain_interpolation(RoadTerrainTraversalState *terrain_state,
                                         int                        sample,
                                         int                        direction)
{
  RoadHillTerrainVariantState *hill_state;
  const RoadHillTerrainDefinition *definition;
  const RoadHillTerrainControl *control;
  UnsignedDivisionResult division_result;
  unsigned int run_count;
  int side;
  int control_index;
  int run;
  int limit;

  hill_state = &terrain_state->variant.hill;
  definition = hill_state->current_definition;

  for(side = ROAD_TERRAIN_SIDE_LEFT;
      side < ROAD_TERRAIN_SIDE_COUNT; side++)
    {
      for(control_index = 0;
          control_index < ROAD_HILL_TERRAIN_CONTROL_COUNT;
          control_index++)
        {
          if(hill_state->interval_ends[side][control_index] > sample &&
             hill_state->interval_starts[side][control_index] < sample)
            {
              continue;
            }

          control = &definition->controls[side][control_index];
          run_count =
            (unsigned int)control->maximum_run -
            (unsigned int)control->minimum_run + 1;
          division_result = unsigned_divide_with_remainder(
            run_count, (unsigned int)rand());
          run = (int)division_result.remainder +
                (int)control->minimum_run;

          if(direction > 0)
            {
              limit = definition->end_sample;
              if(sample + run > limit)
                {
                  run = limit - sample;
                }
              hill_state->interval_starts[side][control_index] = sample;
              hill_state->interval_ends[side][control_index] =
                sample + run;
              if(run == 0)
                {
                  return;
                }
            }
          else
            {
              run = -run;
              limit = definition->start_sample;
              if(sample + run < limit)
                {
                  run = limit - sample;
                }
              hill_state->interval_ends[side][control_index] = sample;
              hill_state->interval_starts[side][control_index] =
                sample + run;
              if(run == 0)
                {
                  return;
                }
            }

          choose_hill_terrain_interpolation_step(
            &hill_state->interpolation_steps[side][control_index],
            &hill_state->current_points[side][control_index],
            control, run);
        }
    }
}


static
void
apply_hill_terrain_interpolation_step(RoadHillTerrainVariantState *hill_state,
                                      int                          direction)
{
  int control_index;

  for(control_index = 0;
      control_index < ROAD_HILL_TERRAIN_CONTROL_COUNT;
      control_index++)
    {
      hill_state->current_points[
        ROAD_TERRAIN_SIDE_LEFT][control_index].x +=
        direction * hill_state->interpolation_steps[
          ROAD_TERRAIN_SIDE_LEFT][control_index].x;
      hill_state->current_points[
        ROAD_TERRAIN_SIDE_LEFT][control_index].y +=
        direction * hill_state->interpolation_steps[
          ROAD_TERRAIN_SIDE_LEFT][control_index].y;
      hill_state->current_points[
        ROAD_TERRAIN_SIDE_RIGHT][control_index].x +=
        direction * hill_state->interpolation_steps[
          ROAD_TERRAIN_SIDE_RIGHT][control_index].x;
      hill_state->current_points[
        ROAD_TERRAIN_SIDE_RIGHT][control_index].y +=
        direction * hill_state->interpolation_steps[
          ROAD_TERRAIN_SIDE_RIGHT][control_index].y;
    }
}


void
advance_hill_road_terrain_traversal(RoadTerrainTraversalState *terrain_state,
                                    int                        direction)
{
  RoadHillTerrainVariantState *hill_state;
  const RoadHillTerrainResource *resource;
  const RoadHillTerrainDefinition *definition;
  int target_sample;
  int definition_count;
  int definition_index;
  int current_sample;

  hill_state = &terrain_state->variant.hill;
  definition = hill_state->current_definition;
  if(definition == 0)
    {
      return;
    }

  target_sample =
    terrain_state->track_position >> TRACK_POSITION_FRACTION_BITS;
  resource = (const RoadHillTerrainResource *)terrain_state->resource;
  definition_count = resource->definition_count;
  if(definition_count > 1)
    {
      if(direction > 0)
        {
          definition_index = hill_state->definition_index;
          if(definition_count - 1 > definition_index)
            {
              if(target_sample >= definition[1].start_sample)
                {
                  hill_state->definition_index = definition_index + 1;
                  definition += 1;
                  hill_state->current_definition = definition;
                }
              schedule_hill_road_terrain_interpolation(
                terrain_state, target_sample, direction);

              current_sample = terrain_state->sample_index;
              terrain_state->sample_index =
                current_sample + ROAD_HILL_FORWARD_SAMPLE_STEP;
              while(current_sample < target_sample)
                {
                  apply_hill_terrain_interpolation_step(
                    hill_state, ROAD_HILL_FORWARD_SAMPLE_STEP);
                  current_sample = terrain_state->sample_index;
                  terrain_state->sample_index =
                    current_sample + ROAD_HILL_FORWARD_SAMPLE_STEP;
                }
            }
        }
      else if(direction < 0)
        {
          definition_index = hill_state->definition_index;
          if(definition_index > 0)
            {
              if(target_sample < definition->start_sample)
                {
                  hill_state->definition_index = definition_index - 1;
                  definition -= 1;
                  hill_state->current_definition = definition;
                }
              schedule_hill_road_terrain_interpolation(
                terrain_state, target_sample, direction);

              current_sample = terrain_state->sample_index;
              terrain_state->sample_index =
                current_sample + ROAD_HILL_REVERSE_SAMPLE_STEP;
              while(current_sample > target_sample)
                {
                  apply_hill_terrain_interpolation_step(
                    hill_state, ROAD_HILL_REVERSE_SAMPLE_STEP);
                  current_sample = terrain_state->sample_index;
                  terrain_state->sample_index =
                    current_sample + ROAD_HILL_REVERSE_SAMPLE_STEP;
                }
            }
        }
    }

  terrain_state->sample_index = target_sample;
}

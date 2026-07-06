#include "bss_late_data.h"
#include "division_helpers.h"
#include "stdlib.h"

/* Reconstructed from the original function at 0x00020170. */

enum RepeatedRoadsideObjectValue
{
  ROAD_OBJECT_SELECTOR_INDEX_MASK = 0x3F,
  ROAD_OBJECT_EDGE_INSET = 0xE0,
  ROAD_OBJECT_DEFAULT_MINIMUM_X = -0x1000,
  ROAD_OBJECT_DEFAULT_MINIMUM_Y = -0x2000,
  ROAD_OBJECT_DEFAULT_MAXIMUM_X = 0x1000,
  ROAD_OBJECT_DEFAULT_MAXIMUM_Y = 0,
  ROAD_OBJECT_DEFAULT_BOUNDS_COUNT = 1
};

void
populate_repeated_roadside_object_group(const RoadObjectTraversalState *object_state,
                                        RoadSegmentLaneRuntime         *lane_runtime)
{
  RepeatedRoadsideObjectGroup *objects;
  SignedDivisionResult row_division;
  UnsignedDivisionResult random_division;
  int row_phase;
  int place_left;
  int position;
  int spacing;
  int index;
  int count;
  unsigned int random_range;

  objects = &lane_runtime->repeated_objects;
  objects->placement_count = 0;
  if(object_state->resource == 0)
    {
      return;
    }

  row_phase = object_state->current_sample -
              object_state->active_start_sample;
  if(object_state->row_spacing_minus_one != 0)
    {
      row_division = signed_divide_with_remainder(
        (unsigned int)(object_state->row_spacing_minus_one + 1),
        (unsigned int)row_phase);
      if(row_division.remainder != 0)
        {
          return;
        }
    }

  if((object_state->resource_selector &
      ROAD_OBJECT_SELECTOR_INDEX_MASK) ==
     ROAD_OBJECT_DISABLED_SELECTOR)
    {
      return;
    }

  place_left = object_state->base_left_side;
  if(object_state->alternate_sides != 0 &&
     (row_phase & 1) == 0)
    {
      place_left = 1 - place_left;
    }

  spacing = object_state->lateral_spacing;
  if(place_left != 0)
    {
      position = lane_runtime->road_left +
                 ROAD_OBJECT_EDGE_INSET - spacing;
    }
  else
    {
      position = lane_runtime->road_right -
                 ROAD_OBJECT_EDGE_INSET + spacing;
    }

  objects->collision_box_count =
    ROAD_OBJECT_DEFAULT_BOUNDS_COUNT;
  objects->bounds[0].left =
    ROAD_OBJECT_DEFAULT_MINIMUM_X;
  objects->bounds[0].top =
    ROAD_OBJECT_DEFAULT_MINIMUM_Y;
  objects->bounds[0].right =
    ROAD_OBJECT_DEFAULT_MAXIMUM_X;
  objects->bounds[0].bottom =
    ROAD_OBJECT_DEFAULT_MAXIMUM_Y;

  index = 0;
  count = object_state->column_count_minus_one + 1;
  do
    {
      objects->lateral_positions[index] = position;
      objects->scale = object_state->scale_eighths;
      objects->flags = 0;
      if(object_state->mirror_sprite != 0)
        {
          objects->flags = ROAD_OBJECT_PLACEMENT_MIRRORED;
        }
      if(place_left == 0)
        {
          objects->flags = (unsigned char)(objects->flags |
                                           ROAD_OBJECT_PLACEMENT_FACES_RIGHT);
        }
      objects->resource_selector =
        object_state->resource_selector;

      if(object_state->randomize_lateral_spacing == 0)
        {
          if(place_left == 0)
            {
              position += spacing;
            }
          else
            {
              position -= spacing;
            }
        }
      else
        {
          random_range = (unsigned int)(
            spacing - ((unsigned int)spacing >> 1) + 1);
          random_division = unsigned_divide_with_remainder(
            random_range, (unsigned int)rand());
          if(place_left == 0)
            {
              position += (int)random_division.remainder +
                          (int)((unsigned int)spacing >> 1);
            }
          else
            {
              position -= (int)random_division.remainder +
                          (int)((unsigned int)spacing >> 1);
            }
        }

      index++;
    } while(index < count);

  objects->placement_count = (unsigned char)count;
}

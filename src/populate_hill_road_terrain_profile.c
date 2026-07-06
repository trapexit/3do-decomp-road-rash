#include "bss_late_data.h"
#include "division_helpers.h"
#include "road_geometry.h"

/* Reconstructed from the original function at 0x0001FB90. */

enum RoadHillTerrainProfileConstants
{
  ROAD_HILL_FIXED_ONE = 0x10000,
  ROAD_HILL_FIXED_FRACTION_BITS = 16,
  ROAD_HILL_SHADING_SCALE_SHIFT = 5,
  ROAD_HILL_MAX_SHADING = 0x1F
};

void
populate_hill_road_terrain_profile(const RoadTerrainTraversalState *terrain_state,
                                   RoadSegmentLaneRuntime          *lane_runtime)
{
  const RoadHillTerrainVariantState *hill_state;
  const RoadHillTerrainPoint *terrain_point;
  SignedDivisionResult division_result;
  int profile_row;
  int side;
  int profile_index;
  int previous_x;
  int previous_y;
  int current_x;
  int current_y;
  int dx;
  int dy;
  int distance;
  int value;

  hill_state = &terrain_state->variant.hill;
  for(profile_row = 0;
      profile_row < ROAD_HILL_TERRAIN_RENDER_POINT_COUNT;
      profile_row++)
    {
      profile_index = profile_row * ROAD_SIDE_COUNT;
      terrain_point = &hill_state->current_points[
        ROAD_TERRAIN_SIDE_LEFT][profile_row];
      lane_runtime->surface_profile[
        profile_index + ROAD_SIDE_LEFT].x =
        lane_runtime->shoulder_left - terrain_point->x;
      lane_runtime->surface_profile[
        profile_index + ROAD_SIDE_LEFT].y =
        terrain_point->y;
      terrain_point = &hill_state->current_points[
        ROAD_TERRAIN_SIDE_RIGHT][profile_row];
      lane_runtime->surface_profile[
        profile_index + ROAD_SIDE_RIGHT].x =
        lane_runtime->shoulder_right + terrain_point->x;
      lane_runtime->surface_profile[
        profile_index + ROAD_SIDE_RIGHT].y =
        terrain_point->y;
    }

  lane_runtime->edge_depth_offsets[ROAD_SIDE_LEFT] = 0;
  lane_runtime->edge_depth_offsets[ROAD_SIDE_RIGHT] = 0;
  lane_runtime->geometry_mode = ROAD_GEOMETRY_MODE_PROFILE;

  lane_runtime->surface_resource_ids[ROAD_SIDE_LEFT] =
    hill_state->current_definition->surface_resource_ids[
      ROAD_TERRAIN_SIDE_LEFT];
  lane_runtime->surface_resource_ids[ROAD_SIDE_RIGHT] =
    hill_state->current_definition->surface_resource_ids[
      ROAD_TERRAIN_SIDE_RIGHT];

  for(side = ROAD_SIDE_LEFT; side < ROAD_SIDE_COUNT; side++)
    {
      for(profile_row = 0;
          profile_row < ROAD_HILL_TERRAIN_RENDER_POINT_COUNT;
          profile_row++)
        {
          profile_index = profile_row * ROAD_SIDE_COUNT + side;
          if(profile_row == 0)
            {
              previous_y = 0;
              previous_x = side == ROAD_SIDE_LEFT
                    ? lane_runtime->shoulder_left
                    : lane_runtime->shoulder_right;
            }
          else
            {
              previous_y = current_y;
              previous_x = current_x;
            }

          current_x =
            lane_runtime->surface_profile[profile_index].x;
          current_y =
            lane_runtime->surface_profile[profile_index].y;

          dx = current_x - previous_x;
          if(dx < 0)
            {
              dx = -dx;
            }
          dy = current_y - previous_y;
          if(dy < 0)
            {
              dy = -dy;
            }

          distance = dx + dy;
          if(dx >= dy)
            {
              distance -= dy >> DIVISION_BIT_1;
            }
          else
            {
              distance -= dx >> DIVISION_BIT_1;
            }

          division_result = signed_divide_with_remainder(
            (unsigned int)distance,
            (unsigned int)(dy << ROAD_HILL_FIXED_FRACTION_BITS));
          value = ROAD_HILL_FIXED_ONE - division_result.quotient;
          if(value < 0)
            {
              value = 0;
            }
          value = (value << ROAD_HILL_SHADING_SCALE_SHIFT) >>
                  ROAD_HILL_FIXED_FRACTION_BITS;
          if(value > ROAD_HILL_MAX_SHADING)
            {
              value = ROAD_HILL_MAX_SHADING;
            }

          lane_runtime->profile_shading[profile_index] = value;
        }
    }
}

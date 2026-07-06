#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RoadProjectionValue
{
  ROAD_PROJECTION_DEPTH_HALF_SHIFT = 1,
  ROAD_PROJECTION_SCREEN_CENTER_X = 0x960000,
  ROAD_PROJECTION_SCREEN_CENTER_Y = 0x5A0000
};

int
project_road_world_point(RoadFixed  world_x,
                         RoadFixed  world_y,
                         int        projection_depth,
                         RoadPoint *screen_point)
{
  int half_depth;
  int scale;
  int result;

  half_depth = projection_depth >> ROAD_PROJECTION_DEPTH_HALF_SHIFT;

  scale = road_renderer_state.reciprocal_table[half_depth];
  result = scale *
           (world_x -
            road_renderer_state.motion.projection_origin_x) +
           ROAD_PROJECTION_SCREEN_CENTER_X;
  screen_point->x = result;

  scale = road_renderer_state.reciprocal_table[half_depth];
  result = scale *
           (road_renderer_state.motion.projection_origin_y - world_y) +
           ROAD_PROJECTION_SCREEN_CENTER_Y;
  screen_point->y = result;

  return result;
}

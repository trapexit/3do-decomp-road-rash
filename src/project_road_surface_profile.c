#include "bss_early_data.h"
#include "bss_late_data.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

int
project_road_surface_profile(RoadRenderSide *road_side)
{
  const RoadLanePoint *source_points;
  RoadRenderPolyline6 *projected_profile;
  RoadRenderPoint *destination_point;
  int projection_scale;
  int horizontal_offset;
  int vertical_offset;
  int maximum_y;
  int point_index;

  source_points = road_side->lane->surface_profile;
  projected_profile =
    (RoadRenderPolyline6 *)(void *)&road_side->mode_geometry[0];
  destination_point = projected_profile->points;
  projection_scale = road_side->projection_scale;
  horizontal_offset = road_side->world_x -
                      road_renderer_state.motion.projection_origin_x;
  vertical_offset = road_renderer_state.motion.projection_origin_y -
                    road_side->world_y;
  maximum_y = road_side->horizon_y;
  point_index = 0;

  if(projection_scale < ROAD_FIXED_ONE)
    {
      do
        {
          int projected_y;

          destination_point->x =
            projection_scale *
            (source_points->x + horizontal_offset) +
            ROAD_PROJECTION_SCREEN_X_CENTER;
          projected_y =
            projection_scale *
            (vertical_offset - source_points->y) +
            ROAD_PROJECTION_SCREEN_Y_ORIGIN;
          if(maximum_y < projected_y)
            {
              maximum_y = projected_y;
            }
          destination_point->y = projected_y;
          destination_point++;
          source_points++;
          point_index++;
        } while(point_index < ROAD_SURFACE_PROFILE_POINT_COUNT);
      return maximum_y;
    }

  do
    {
      int horizontal_distance;
      int projected_x;
      int projected_y;

      horizontal_distance = source_points->x + horizontal_offset;
      projected_x = projection_scale * horizontal_distance;
      if(horizontal_distance < 0)
        {
          if(projected_x < 0)
            {
              destination_point->x =
                projected_x + ROAD_PROJECTION_SCREEN_X_CENTER;
            }
          else
            {
              destination_point->x = ROAD_PROJECTION_PRODUCT_MINIMUM;
            }
        }
      else
        {
          projected_x += ROAD_PROJECTION_SCREEN_X_CENTER;
          if(projected_x < 0)
            {
              destination_point->x = ROAD_PROJECTION_PRODUCT_LIMIT;
            }
          else
            {
              destination_point->x = projected_x;
            }
        }

      projected_y =
        projection_scale * (vertical_offset - source_points->y) +
        ROAD_PROJECTION_SCREEN_Y_ORIGIN;
      if(maximum_y < projected_y)
        {
          maximum_y = projected_y;
        }
      destination_point->y = projected_y;
      destination_point++;
      source_points++;
      point_index++;
    } while(point_index < ROAD_SURFACE_PROFILE_POINT_COUNT);

  return maximum_y;
}

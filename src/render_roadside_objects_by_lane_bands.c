#include "road_geometry.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"

enum RoadsideLaneBandConstant
{
  ROAD_SCREEN_CENTER_X = 0x960000,
  ROAD_OBJECT_MINIMUM_LATERAL_POSITION = -0x2710,
  ROAD_OBJECT_MAXIMUM_LATERAL_POSITION = 0x2710,
  ROAD_OBJECT_DRAW_COMMAND_COUNT = 5,
  ROAD_LANE_INNER_LEFT_EDGE = 2,
  ROAD_LANE_INNER_RIGHT_EDGE = 3,
  ROAD_LEFT_EDGE_DRAW_FLAG = 1,
  ROAD_RIGHT_EDGE_DRAW_FLAG = 2,
  ROAD_DRAW_ORDER_RIGHT_ONLY = 2
};

enum RoadsideLaneBandCommand
{
  DRAW_OBJECTS_LEFT_OF_SHOULDER = 0,
  DRAW_LEFT_EDGE = 1,
  DRAW_OBJECTS_BETWEEN_SHOULDERS = 2,
  DRAW_RIGHT_EDGE = 3,
  DRAW_OBJECTS_RIGHT_OF_SHOULDER = 4
};

int
render_roadside_objects_by_lane_bands(RoadRenderSide *road_side)
{
  RoadSegmentLaneRuntime *lane;
  const signed char *commands;
  int command_index;
  int command;

  lane = road_side->lane;
  commands = road_draw_order[1];

  if(road_side->lane_edges[ROAD_LANE_INNER_LEFT_EDGE].x >=
     ROAD_SCREEN_CENTER_X)
    {
      commands = road_draw_order[0];
    }
  else if(road_side->lane_edges[ROAD_LANE_INNER_RIGHT_EDGE].x <=
          ROAD_SCREEN_CENTER_X)
    {
      commands = road_draw_order[ROAD_DRAW_ORDER_RIGHT_ONLY];
    }

  command_index = 0;
  do
    {
      command = *commands++;
      switch(command)
        {
        case DRAW_OBJECTS_LEFT_OF_SHOULDER:
          render_roadside_objects_in_range(
            road_side, ROAD_OBJECT_MINIMUM_LATERAL_POSITION,
            lane->shoulder_left);
          break;
        case DRAW_LEFT_EDGE:
          if((lane->edge_collision_flags &
              ROAD_LEFT_EDGE_DRAW_FLAG) != 0)
            {
              render_roadside_edge_cel(road_side, ROAD_SIDE_LEFT);
            }
          break;
        case DRAW_OBJECTS_BETWEEN_SHOULDERS:
          render_roadside_objects_in_range(
            road_side, lane->shoulder_left, lane->shoulder_right);
          break;
        case DRAW_RIGHT_EDGE:
          if((lane->edge_collision_flags &
              ROAD_RIGHT_EDGE_DRAW_FLAG) != 0)
            {
              render_roadside_edge_cel(road_side, ROAD_SIDE_RIGHT);
            }
          break;
        case DRAW_OBJECTS_RIGHT_OF_SHOULDER:
          render_roadside_objects_in_range(
            road_side, lane->shoulder_right,
            ROAD_OBJECT_MAXIMUM_LATERAL_POSITION);
          break;
        }

      command_index++;
    } while(command_index < ROAD_OBJECT_DRAW_COMMAND_COUNT);

  return 0;
}

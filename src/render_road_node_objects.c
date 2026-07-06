#include "roadside_object_rendering.h"

enum RoadNodeObjectRenderConstant
{
  ROAD_SINGLE_SIDE_NODE = 1,
  ROAD_SPLIT_OBJECT_BAND_COLLISION_KIND = 5,
  ROAD_SCREEN_CENTER_X = 0x960000,
  ROAD_OBJECT_MINIMUM_LATERAL_POSITION = -0x2710,
  ROAD_OBJECT_MAXIMUM_LATERAL_POSITION = 0x2710,
  ROAD_LEFT_SIDE_ORDER_POINT = 3,
  ROAD_RIGHT_SIDE_ORDER_POINT = 2,
  ROAD_SIGN_BIT_SHIFT = 31
};

static
int
midpoint_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> ROAD_SIGN_BIT_SHIFT)) >> 1;
}


static
int
render_road_side_objects(RoadRenderSide *road_side)
{
  if(road_side->lane->collision_kind ==
     ROAD_SPLIT_OBJECT_BAND_COLLISION_KIND)
    {
      return render_roadside_objects_by_lane_bands(road_side);
    }

  return render_roadside_objects_in_range(
    road_side, ROAD_OBJECT_MINIMUM_LATERAL_POSITION,
    ROAD_OBJECT_MAXIMUM_LATERAL_POSITION);
}


static
int
render_active_road_side_objects(RoadRenderSide *road_side)
{
  if(!road_side->active)
    {
      return 0;
    }
  return render_road_side_objects(road_side);
}


int
render_road_node_objects(RoadRenderNode *node)
{
  RoadRenderSide *first_side;
  RoadRenderSide *second_side;
  RoadRenderSide *swap_side;
  int side_midpoint;

  first_side = node->left_side;
  if(node->node_type == ROAD_SINGLE_SIDE_NODE)
    {
      return render_road_side_objects(first_side);
    }

  second_side = node->right_side;
  side_midpoint = midpoint_towards_zero(
    first_side->lane_edges[ROAD_LEFT_SIDE_ORDER_POINT].x +
    second_side->lane_edges[ROAD_RIGHT_SIDE_ORDER_POINT].x);
  if(side_midpoint > ROAD_SCREEN_CENTER_X)
    {
      swap_side = first_side;
      first_side = second_side;
      second_side = swap_side;
    }

  render_active_road_side_objects(first_side);
  return render_active_road_side_objects(second_side);
}

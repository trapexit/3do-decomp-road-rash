#include "cel_dimension_resource.h"
#include "display_cel_runtime.h"
#include "family_resource_runtime.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"

enum RoadsideEdgeCelConstant
{
  ROAD_EDGE_FAMILY_INDEX = 6,
  ROAD_EDGE_SPAN_SCALE_SHIFT = 2,
  ROAD_EDGE_FLAG_PRESERVE_MASK = 3,
  ROAD_EDGE_FLAG_SPAN_SHIFT = 2,
  ROAD_EDGE_TRANSITION_WORLD_SPACE = 4,
  ROAD_EDGE_LANE_POINT_BASE = 2,
  ROAD_EDGE_SCREEN_CENTER_X = 0x960000,
  ROAD_EDGE_SCREEN_RIGHT_X = 0x12C0000,
  ROAD_EDGE_PIXC_FORWARD = 0x10,
  ROAD_EDGE_PIXC_REVERSED = 0x1F
};

static
int
divide_edge_span_by_four(int value)
{
  if(value < 0)
    {
      value += (1 << ROAD_EDGE_SPAN_SCALE_SHIFT) - 1;
    }
  return value >> ROAD_EDGE_SPAN_SCALE_SHIFT;
}


static
int
lane_edge_position(const RoadSegmentLaneRuntime *lane,
                   RoadSide                      side)
{
  if(side == ROAD_SIDE_LEFT)
    {
      return lane->road_left;
    }
  return lane->road_right;
}


CCB *
render_roadside_edge_cel(RoadRenderSide *road_side,
                         RoadSide        side)
{
  RoadSegmentLaneRuntime *lane;
  RoadSegmentRuntime *segment;
  RoadRenderSide *previous_side;
  CelDimensionResource *resource;
  DisplayQuad quad;
  CCB *cel;
  int collision_inset;
  int source_span;
  int previous_x;
  int previous_y;
  int current_x;
  int current_y;
  int projected_width;
  int projected_height;

  lane = road_side->lane;
  collision_inset = lane->collision_inset;
  if(side == ROAD_SIDE_LEFT)
    {
      collision_inset = -collision_inset;
    }

  previous_side = road_side->next_sides[ROAD_SIDE_LEFT];
  resource = (CelDimensionResource *)
             lookup_family_resource_entry_for_render(
    lane->surface_selector, ROAD_EDGE_FAMILY_INDEX);
  if(resource == 0)
    {
      return 0;
    }

  source_span = 1 << resource->vertical_base_exponent;
  source_span <<= resource->vertical_count;
  source_span <<= ROAD_EDGE_SPAN_SCALE_SHIFT;
  lane->edge_collision_flags = (unsigned char)(
    (lane->edge_collision_flags & ROAD_EDGE_FLAG_PRESERVE_MASK) |
    (divide_edge_span_by_four(source_span) <<
      ROAD_EDGE_FLAG_SPAN_SHIFT));

  segment = (RoadSegmentRuntime *)lane->resource_handle;
  if(segment->transition_type == ROAD_EDGE_TRANSITION_WORLD_SPACE)
    {
      previous_x = lane_edge_position(lane, side) +
                   previous_side->world_x -
                   road_renderer_state.motion.projection_origin_x;
      previous_x = previous_side->projection_scale * previous_x +
                   ROAD_EDGE_SCREEN_CENTER_X;
    }
  else
    {
      previous_x =
        previous_side->lane_edges[ROAD_EDGE_LANE_POINT_BASE + side].x +
        previous_side->projection_scale * collision_inset;
    }
  previous_y =
    previous_side->lane_edges[ROAD_EDGE_LANE_POINT_BASE + side].y;

  current_x =
    road_side->lane_edges[ROAD_EDGE_LANE_POINT_BASE + side].x +
    road_side->projection_scale * collision_inset;
  current_y =
    road_side->lane_edges[ROAD_EDGE_LANE_POINT_BASE + side].y;

  quad.top_left.x = current_x;
  quad.top_left.y =
    current_y - road_side->projection_scale * source_span;
  quad.top_right.x = previous_x;
  quad.top_right.y =
    previous_y - previous_side->projection_scale * source_span;
  quad.bottom_right.x = previous_x;
  quad.bottom_right.y = previous_y;
  quad.bottom_left.x = current_x;
  quad.bottom_left.y = current_y;

  projected_width =
    (current_x - previous_x) >> DISPLAY_COORDINATE_FRACTION_SHIFT;
  if(projected_width < 0)
    {
      projected_width = -projected_width;
    }
  projected_height =
    (road_side->projection_scale * source_span) >>
    DISPLAY_COORDINATE_FRACTION_SHIFT;

  cel = select_cel_for_dimensions(
    resource, projected_width, projected_height);
  if(cel == 0)
    {
      cel = display_runtime_globals.fallback_ccb;
    }

  if((current_x > ROAD_EDGE_SCREEN_RIGHT_X &&
      previous_x > ROAD_EDGE_SCREEN_RIGHT_X) ||
     (current_x < 0 && previous_x < 0))
    {
      return cel;
    }

  if((side == ROAD_SIDE_LEFT && current_x >= previous_x) ||
     (side == ROAD_SIDE_RIGHT && current_x <= previous_x))
    {
      cel->ccb_PIXC = ROAD_EDGE_PIXC_FORWARD;
    }
  else
    {
      cel->ccb_PIXC = ROAD_EDGE_PIXC_REVERSED;
    }

  return append_mapped_quad_cel(cel, &quad);
}

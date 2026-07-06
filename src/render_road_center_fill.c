#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RoadCenterFillValue
{
  ROAD_CENTER_PRIMARY_COLOR = 0,
  ROAD_CENTER_BRIDGE_COLOR = 1,
  ROAD_CENTER_DUAL_LANE_COUNT = 2,
  ROAD_CENTER_TRANSITION_STEP_MINIMUM = 0x18,
  ROAD_CENTER_EDGE_WIDTH = 12,
  ROAD_CENTER_CLIP_DEPTH = ROAD_SUBDIVISION_MIN_DEPTH - 1,
  ROAD_CENTER_BRIDGE_TRANSITION_TYPE = 4,
  ROAD_CENTER_TRANSITION_TYPE_BIAS = 6,
  ROAD_CENTER_SELECTOR_SPAN = 2,
  ROAD_CENTER_SELECTOR_LEVEL = 1
};

int
render_road_span_geometry(RoadRenderNode *node,
                          int             selector,
                          int             level);
int
render_road_profile_node(RoadRenderNode *node,
                         int             selector,
                         int             level);
int
render_road_side_geometry(RoadRenderSide *road_side,
                          RoadSide        side);
int
clip_road_quad_to_near_depth(RoadRenderSide *road_side,
                             RoadQuad       *quad,
                             RoadFixed       source_offset,
                             RoadFixed       neighbor_offset,
                             RoadFixed      *projected_width,
                             RoadFixed       source_width);

static
CCB *
append_road_center_quad(const RoadQuad *quad,
                        unsigned int    color)
{
  return append_solid_quad_cel(
    (const DisplayQuad *)(const void *)quad, color);
}


static
void
copy_road_render_point(RoadPoint             *destination,
                       const RoadRenderPoint *source)
{
  destination->x = source->x;
  destination->y = source->y;
}


int
render_road_center_fill(RoadRenderNode *node)
{
  RoadRenderNode *neighbor_node;
  RoadRenderSide *neighbor_left;
  RoadRenderSide *neighbor_right;
  RoadRenderSide *right;
  RoadRenderSide *left;
  RoadSegmentRuntime *neighbor_segment;
  int *colors;
  int left_width;
  int neighbor_width;
  RoadFixed projected_width;
  RoadQuad left_quad;
  RoadQuad right_quad;
  RoadQuad bridge_quad;

  neighbor_node = node + 1;
  neighbor_left = neighbor_node->left_side;
  neighbor_right = neighbor_node->right_side;
  neighbor_segment = neighbor_node->segment;
  right = node->right_side;
  left = node->left_side;
  colors = road_surface_colors;

  if(neighbor_segment->lane_count == ROAD_CENTER_DUAL_LANE_COUNT &&
     neighbor_segment->left_step >
     ROAD_CENTER_TRANSITION_STEP_MINIMUM)
    {
      left_width = left->projection_scale * ROAD_CENTER_EDGE_WIDTH;
      neighbor_width =
        neighbor_left->projection_scale * ROAD_CENTER_EDGE_WIDTH;

      copy_road_render_point(
        &left_quad.top_left,
        &neighbor_left->lane_edges[ROAD_LANE_EDGE_RIGHT]);
      left_quad.top_right.x = left_quad.top_left.x + neighbor_width;
      left_quad.top_right.y = left_quad.top_left.y;
      left_quad.bottom_right.x =
        left->lane_edges[ROAD_LANE_EDGE_RIGHT].x + left_width;
      left_quad.bottom_right.y = left->horizon_y;
      copy_road_render_point(
        &left_quad.bottom_left,
        &left->lane_edges[ROAD_LANE_EDGE_RIGHT]);

      if(left->projection_depth <= ROAD_CENTER_CLIP_DEPTH)
        {
          clip_road_quad_to_near_depth(
            left, &left_quad, left->lane->road_right,
            neighbor_left->lane->road_right, &projected_width,
            ROAD_CENTER_EDGE_WIDTH);
        }

      right_quad.top_left.x =
        neighbor_right->lane_edges[ROAD_LANE_EDGE_LEFT].x -
        neighbor_width;
      right_quad.top_left.y = neighbor_right->horizon_y;
      copy_road_render_point(
        &right_quad.top_right,
        &neighbor_right->lane_edges[ROAD_LANE_EDGE_LEFT]);
      copy_road_render_point(
        &right_quad.bottom_right,
        &right->lane_edges[ROAD_LANE_EDGE_LEFT]);
      right_quad.bottom_left.x =
        right->lane_edges[ROAD_LANE_EDGE_LEFT].x - left_width;
      right_quad.bottom_left.y = right->horizon_y;

      if(right->projection_depth <= ROAD_CENTER_CLIP_DEPTH)
        {
          clip_road_quad_to_near_depth(
            right, &right_quad,
            right->lane->road_left - ROAD_CENTER_EDGE_WIDTH,
            neighbor_right->lane->road_left - ROAD_CENTER_EDGE_WIDTH,
            &projected_width, ROAD_CENTER_EDGE_WIDTH);
        }

      bridge_quad.top_left = left_quad.top_right;
      bridge_quad.top_right = left_quad.bottom_right;
      bridge_quad.bottom_right = right_quad.top_left;
      bridge_quad.bottom_left = right_quad.bottom_left;

      append_road_center_quad(
        &bridge_quad, colors[ROAD_CENTER_BRIDGE_COLOR]);
      append_road_center_quad(
        &left_quad, colors[ROAD_CENTER_PRIMARY_COLOR]);
      return (int)append_road_center_quad(
        &right_quad, colors[ROAD_CENTER_PRIMARY_COLOR]);
    }

  if(node->segment->transition_type ==
     ROAD_CENTER_BRIDGE_TRANSITION_TYPE)
    {
      left_quad.top_left.x =
        left->connection_geometry[ROAD_CONNECTION_RIGHT_X];
      left_quad.top_left.y =
        left->connection_geometry[ROAD_CONNECTION_CENTER_Y];
      left_quad.top_right.x =
        right->connection_geometry[ROAD_CONNECTION_LEFT_X];
      left_quad.top_right.y =
        right->connection_geometry[ROAD_CONNECTION_CENTER_Y];
    }
  else
    {
      copy_road_render_point(
        &left_quad.top_left,
        &neighbor_left->lane_edges[ROAD_LANE_EDGE_RIGHT]);
      copy_road_render_point(
        &left_quad.top_right,
        &neighbor_right->lane_edges[ROAD_LANE_EDGE_LEFT]);
    }

  copy_road_render_point(
    &left_quad.bottom_right,
    &right->lane_edges[ROAD_LANE_EDGE_LEFT]);
  copy_road_render_point(
    &left_quad.bottom_left,
    &left->lane_edges[ROAD_LANE_EDGE_RIGHT]);

  if(left->projection_depth <= ROAD_CENTER_CLIP_DEPTH)
    {
      clip_road_quad_to_near_depth(
        left, &left_quad, left->lane->road_right,
        neighbor_left->lane->road_right, &projected_width,
        node->segment->left_step);
    }

  return (int)append_road_center_quad(
    &left_quad, colors[ROAD_CENTER_PRIMARY_COLOR]);
}


int
render_road_node_center_geometry(RoadRenderNode *node)
{
  RoadSegmentRuntime *segment;
  int level;
  int selector;
  int geometry_mode;

  segment = node->segment;
  selector = ROAD_CENTER_SELECTOR_SPAN;
  level = segment->transition_amount;

  if(segment->transition_type == ROAD_CENTER_TRANSITION_TYPE_BIAS)
    {
      level += 1;
    }
  if(level == 0)
    {
      selector = ROAD_CENTER_SELECTOR_LEVEL;
    }
  else if(level > 0)
    {
      selector = ROAD_CENTER_PRIMARY_COLOR;
    }

  geometry_mode = segment->selected_lanes[ROAD_SIDE_LEFT]->geometry_mode;
  if(geometry_mode == ROAD_GEOMETRY_MODE_TEXTURED)
    {
      return render_road_span_geometry(node, selector, level);
    }
  if(geometry_mode == ROAD_GEOMETRY_MODE_PROFILE)
    {
      return render_road_profile_node(node, selector, level);
    }

  render_road_side_geometry(node->left_side, ROAD_SIDE_RIGHT);
  return render_road_side_geometry(node->right_side, ROAD_SIDE_LEFT);
}

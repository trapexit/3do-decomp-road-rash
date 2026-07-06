/* Reconstructed from grouped blocks in the original function at 0x0002E940. */



#include "bss_early_data.h"
#include "bss_late_data.h"
#include "cel_dimension_resource.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "road_surface_cel_selection.h"
#include "rw_semantic_data.h"

typedef struct RoadsideSegmentJoinContext
{
  RoadStripDrawContext draw;
  RoadQuad screen_quad;
} RoadsideSegmentJoinContext;

enum RoadsideSegmentJoinValue
{
  ROAD_EDGE_CEL_RESOURCE_GROUP = 5,
  ROAD_EDGE_CEL_RESOURCE_VARIANT = 0,
  ROAD_PROFILE_CENTER_CEL_INDEX = 2,
  ROAD_PROFILE_DUAL_LANE_COUNT = 2,
  ROAD_PROFILE_OUTER_BAND_COUNT = 3,
  ROAD_PROFILE_SELECTOR_CENTER_FILL = 0,
  ROAD_PROFILE_SELECTOR_FIRST = 1,
  ROAD_PROFILE_SELECTOR_LAST = 2
};

CCB *
lookup_family_resource_child_for_render(int resource_id,
                                        int resource_group,
                                        int resource_variant);
int
road_quad_is_horizontally_visible(const RoadRenderSide *road_side,
                                  const RoadQuad       *quad,
                                  int                   reverse_direction);
void
render_road_profile_bands(RoadRenderSide *road_side,
                          int             band_count,
                          RoadSide        side);
void
subdivide_three_point_road_strip(RoadStripDrawContext *context);

static
int
absolute_road_join_delta(int value)
{
  if(value < 0)
    {
      return -value;
    }
  return value;
}


static
int
decode_signed_road_profile_count(unsigned char value)
{
  return (int)(signed char)value;
}


void
render_roadside_segment_join(RoadRenderSide *road_side,
                             RoadSide        side)
{
  RoadRenderSide *neighbor;
  RoadSegmentLaneRuntime *lane;
  RoadSegmentLaneRuntime *neighbor_lane;
  RoadStripDrawContext *draw;
  RoadQuad *screen_quad;
  CCB **cel_slot;
  CCB *cel;
  int neighbor_mode;
  int width;
  int height;
  RoadsideSegmentJoinContext context;

  neighbor = road_side->next_sides[side];
  lane = road_side->lane;
  neighbor_lane = neighbor->lane;
  draw = &context.draw;
  screen_quad = &context.screen_quad;
  neighbor_mode = neighbor_lane->geometry_mode;

  draw->world_quad.top_left.x =
    (side == ROAD_SIDE_LEFT ? neighbor_lane->road_left
                                : neighbor_lane->road_right) +
    neighbor->world_x;
  draw->world_quad.top_left.y = neighbor->world_y;
  if(neighbor_mode == ROAD_GEOMETRY_MODE_PROFILE ||
     neighbor_mode == ROAD_GEOMETRY_MODE_TEXTURED)
    {
      if(road_side->projection_depth < ROAD_SUBDIVISION_DEPTH_LIMIT)
        {
          draw->world_quad.top_right.x =
            (side == ROAD_SIDE_LEFT ? neighbor_lane->shoulder_left
                                        : neighbor_lane->shoulder_right) +
            neighbor->world_x;
        }
      screen_quad->top_right.x =
        neighbor->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side].x;
    }
  else
    {
      if(road_side->projection_depth < ROAD_SUBDIVISION_DEPTH_LIMIT)
        {
          draw->world_quad.top_right.x =
            neighbor_lane->edge_shapes[side].inner_offset +
            neighbor->world_x;
        }
      screen_quad->top_right.x =
        neighbor->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR + side].x;
    }
  screen_quad->top_left.x = neighbor->lane_edges[side].x;
  screen_quad->top_left.y = ROAD_FIXED_FLOOR_QUARTER(
    neighbor->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side].y);

  if(lane->geometry_mode == ROAD_GEOMETRY_MODE_PROFILE)
    {
      if(road_side->projection_depth < ROAD_SUBDIVISION_DEPTH_LIMIT)
        {
          draw->world_quad.bottom_right.x =
            (side == ROAD_SIDE_LEFT ? lane->shoulder_left
                                        : lane->shoulder_right) +
            road_side->world_x;
          draw->world_quad.bottom_left.x =
            (side == ROAD_SIDE_LEFT ? lane->road_left
                                        : lane->road_right) +
            road_side->world_x;
          draw->world_quad.bottom_left.y = road_side->world_y;
        }
      screen_quad->bottom_right.x =
        road_side->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side].x;
    }
  else
    {
      if(road_side->projection_depth < ROAD_SUBDIVISION_DEPTH_LIMIT)
        {
          draw->world_quad.bottom_right.x =
            lane->edge_shapes[side].inner_offset + road_side->world_x;
          draw->world_quad.bottom_left.x =
            (side == ROAD_SIDE_LEFT ? lane->road_left
                                        : lane->road_right) +
            road_side->world_x;
          draw->world_quad.bottom_left.y = road_side->world_y;
        }
      screen_quad->bottom_right.x =
        road_side->mode_geometry[
          ROAD_MODE_GEOMETRY_LEFT_NEAR + side].x;
    }
  screen_quad->bottom_left.x = road_side->lane_edges[side].x;
  screen_quad->bottom_left.y = ROAD_FIXED_FLOOR_QUARTER(
    road_side->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side].y);

  /* Curved joins can re-enter the screen on their near row. */
  if(road_quad_is_horizontally_visible(
       road_side, screen_quad, side == ROAD_SIDE_LEFT) == 0)
    {
      return;
    }

  if(side == ROAD_SIDE_LEFT)
    {
      if(lane->left_width_cells + lane->right_width_cells > 0)
        {
          screen_quad->top_right.x -= ROAD_FIXED_ONE;
          screen_quad->bottom_right.x -= ROAD_FIXED_ONE;
        }
    }
  else
    {
      if(lane->left_width_cells + lane->right_width_cells > 0)
        {
          screen_quad->top_right.x += ROAD_FIXED_ONE;
          screen_quad->bottom_right.x += ROAD_FIXED_ONE;
        }
    }

  width = absolute_road_join_delta(screen_quad->top_right.x -
                                   screen_quad->top_left.x);
  height = absolute_road_join_delta(screen_quad->bottom_left.y -
                                    screen_quad->top_left.y);

  cel_slot = (CCB **)&road_side->texture_cache->edge_sources[0][side];
  if(*cel_slot == 0)
    {
      *cel_slot = lookup_family_resource_child_for_render(
        lane->edge_resource_ids[side], ROAD_EDGE_CEL_RESOURCE_GROUP,
        ROAD_EDGE_CEL_RESOURCE_VARIANT);
    }

  cel = select_cel_for_dimensions(*cel_slot, ROAD_FIXED_TO_INTEGER(width),
                                  ROAD_FIXED_TO_INTEGER(height));
  if(road_side->projection_depth >= ROAD_SUBDIVISION_DEPTH_LIMIT)
    {
      append_horizontal_quad_cel(
        cel, (const DisplayQuad *)(const void *)screen_quad);
      return;
    }

  draw->side = side;
  draw->projection_depth = neighbor->projection_depth;
  draw->cel = cel;
  draw->screen_quad = screen_quad;
  subdivide_three_point_road_strip(draw);
}


void
render_road_profile_center_band(RoadRenderSide *left,
                                RoadRenderSide *right)
{
  RoadRenderSide *neighbor_left;
  RoadRenderSide *neighbor_right;
  RoadQuad quad;
  CCB *cel;

  neighbor_left = left->next_sides[ROAD_SIDE_RIGHT];
  neighbor_right = right->next_sides[ROAD_SIDE_LEFT];

  quad.top_left.x =
    neighbor_left->transition_geometry[ROAD_TRANSITION_RIGHT_X];
  quad.top_left.y =
    neighbor_left->transition_geometry[ROAD_TRANSITION_RIGHT_Y];
  quad.top_right.x =
    neighbor_right->transition_geometry[ROAD_TRANSITION_LEFT_X];
  quad.top_right.y =
    neighbor_right->transition_geometry[ROAD_TRANSITION_LEFT_Y];
  quad.bottom_right.x =
    right->transition_geometry[ROAD_TRANSITION_LEFT_X];
  quad.bottom_right.y =
    right->transition_geometry[ROAD_TRANSITION_LEFT_Y];
  quad.bottom_left.x =
    left->transition_geometry[ROAD_TRANSITION_RIGHT_X];
  quad.bottom_left.y =
    left->transition_geometry[ROAD_TRANSITION_RIGHT_Y];

  if(road_quad_is_horizontally_visible(
       0, &quad, ROAD_PROFILE_SELECTOR_CENTER_FILL) != 0)
    {
      cel = select_road_surface_cel(
        left, ROAD_SIDE_RIGHT, &quad, ROAD_PROFILE_CENTER_CEL_INDEX);
      append_mapped_quad_cel(
        cel, (const DisplayQuad *)(const void *)&quad);
    }
}


void
render_road_profile_node(RoadRenderNode *node,
                         int             selector,
                         int             transition_level)
{
  RoadSegmentRuntime *segment;
  RoadSegmentRuntime *neighbor_segment;
  RoadRenderSide *left;
  RoadRenderSide *right;
  RoadSegmentLaneRuntime *left_lane;
  int left_level;
  int right_level;
  int low;
  int high;

  (void)transition_level;
  segment = node->segment;
  neighbor_segment = segment->next;
  left = node->left_side;
  right = node->right_side;
  left_lane = left->lane;

  if(selector == ROAD_PROFILE_SELECTOR_CENTER_FILL)
    {
      render_road_center_fill(node);
      return;
    }
  if(selector != ROAD_PROFILE_SELECTOR_FIRST &&
     selector != ROAD_PROFILE_SELECTOR_LAST)
    {
      return;
    }

  if(left->active != 0)
    {
      render_roadside_segment_join(left, ROAD_SIDE_RIGHT);
    }
  if(right->active != 0)
    {
      render_roadside_segment_join(right, ROAD_SIDE_LEFT);
    }

  left_level = decode_signed_road_profile_count(
    neighbor_segment->left_transition_profile_count);
  right_level = decode_signed_road_profile_count(
    neighbor_segment->right_transition_profile_count);
  if(neighbor_segment->lane_count != ROAD_PROFILE_DUAL_LANE_COUNT)
    {
      if(left_lane->previous_links[ROAD_SIDE_LEFT] != left_lane)
        {
          left_level = ROAD_PROFILE_OUTER_BAND_COUNT;
          right_level = ROAD_PROFILE_SELECTOR_CENTER_FILL;
        }
      else
        {
          left_level = ROAD_PROFILE_SELECTOR_CENTER_FILL;
          right_level = ROAD_PROFILE_OUTER_BAND_COUNT;
        }
    }

  low = decode_signed_road_profile_count(
    segment->left_transition_profile_count);
  if(low < left_level)
    {
      low = left_level;
    }
  high = decode_signed_road_profile_count(
    segment->right_transition_profile_count);
  if(high < right_level)
    {
      high = right_level;
    }

  if(road_renderer_state.road_side_selector == ROAD_SIDE_LEFT)
    {
      if(right->active != 0)
        {
          render_road_profile_bands(right, high,
                                    ROAD_SIDE_LEFT);
        }
      if(low > ROAD_PROFILE_OUTER_BAND_COUNT)
        {
          render_road_profile_center_band(left, right);
        }
      if(left->active != 0)
        {
          render_road_profile_bands(left, low,
                                    ROAD_SIDE_RIGHT);
        }
    }
  else
    {
      if(left->active != 0)
        {
          render_road_profile_bands(left, low,
                                    ROAD_SIDE_RIGHT);
        }
      if(low > ROAD_PROFILE_OUTER_BAND_COUNT)
        {
          render_road_profile_center_band(left, right);
        }
      if(right->active != 0)
        {
          render_road_profile_bands(right, high,
                                    ROAD_SIDE_LEFT);
        }
    }
}

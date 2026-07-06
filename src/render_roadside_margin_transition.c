#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"

enum RoadsideMarginTransitionConstant
{
  ROAD_MARGIN_DEPTH_INDEX_SHIFT = 1,
  ROAD_MARGIN_PROJECTION_SCALE_SHIFT = 8,
  ROAD_MARGIN_NEAR_REPEAT_COUNT = 16,
  ROAD_MARGIN_FAR_REPEAT_COUNT = 8,
  ROAD_MARGIN_INNER_LANE_POINT_BASE = 2,
  ROAD_MARGIN_PRIMARY_CEL_CHILD = 2,
  ROAD_MARGIN_FILL_CEL_CHILD = 3
};

char *
select_cached_description_cel(char **cache_slots,
                              char  *description,
                              int    child_index,
                              int    side,
                              int   *quad);

CCB *
render_roadside_margin_transition(RoadRenderSide *road_side,
                                  RoadSide        side)
{
  RoadRenderSide *previous_side;
  RoadSegmentLaneRuntime *lane;
  RoadTextureCache *cache;
  RoadCelBounds bounds;
  DisplayQuad quad;
  CCB *cel;
  CCB *result;
  int *reciprocal_table;
  int margin_depth;
  int previous_span;
  int current_span;
  int repeat_count;
  int repeat_index;

  previous_side = road_side->next_sides[side];
  lane = road_side->lane;
  margin_depth = lane->edge_margins[side];
  if(margin_depth < 0)
    {
      margin_depth = -margin_depth;
    }

  reciprocal_table = road_renderer_state.reciprocal_table;
  cache = road_side->texture_cache;

  if((lane->surface_flags & ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
    {
      previous_span =
        reciprocal_table[
          previous_side->projection_depth >>
          ROAD_MARGIN_DEPTH_INDEX_SHIFT] <<
          ROAD_MARGIN_PROJECTION_SCALE_SHIFT;
      current_span =
        reciprocal_table[
          (previous_side->projection_depth - margin_depth) >>
          ROAD_MARGIN_DEPTH_INDEX_SHIFT] <<
          ROAD_MARGIN_PROJECTION_SCALE_SHIFT;

      quad.top_left.x =
        previous_side->lane_edges[
          ROAD_MARGIN_INNER_LANE_POINT_BASE + side].x;
      if(side == ROAD_SIDE_LEFT)
        {
          quad.top_left.x -= previous_span;
        }
      quad.top_left.y = previous_side->lane_edges[side].y;
      quad.top_right.x = quad.top_left.x + previous_span;
      quad.top_right.y = quad.top_left.y;

      quad.bottom_left.x = road_side->attachment_points[side].x;
      quad.bottom_left.y = road_side->attachment_points[side].y;
      if(side == ROAD_SIDE_LEFT)
        {
          quad.bottom_left.x -= current_span;
        }
      quad.bottom_right.x = quad.bottom_left.x + current_span;
      quad.bottom_right.y = quad.bottom_left.y;

      return append_horizontal_quad_cel(
        (CCB *)select_cached_description_cel(
          (char **)cache->join_sources[0],
          (char *)lane->edge_resource_ids,
          ROAD_MARGIN_PRIMARY_CEL_CHILD, side, (int *)&quad),
        &quad);
    }

  if(road_side->owner_node->depth <=
     road_renderer_state.texture_resource_threshold)
    {
      repeat_count = ROAD_MARGIN_NEAR_REPEAT_COUNT;
    }
  else
    {
      repeat_count = ROAD_MARGIN_FAR_REPEAT_COUNT;
    }

  current_span =
    reciprocal_table[
      (road_side->projection_depth + margin_depth) >>
      ROAD_MARGIN_DEPTH_INDEX_SHIFT] <<
      ROAD_MARGIN_PROJECTION_SCALE_SHIFT;
  previous_span =
    reciprocal_table[
      road_side->projection_depth >>
      ROAD_MARGIN_DEPTH_INDEX_SHIFT] <<
      ROAD_MARGIN_PROJECTION_SCALE_SHIFT;

  bounds.left =
    road_side->lane_edges[
      ROAD_MARGIN_INNER_LANE_POINT_BASE + side].x;
  if(side == ROAD_SIDE_LEFT)
    {
      bounds.left -= previous_span;
    }
  bounds.right = bounds.left + previous_span;
  bounds.top = road_side->lane_edges[side].y;
  bounds.bottom = road_side->horizon_y;

  quad.top_left.x = road_side->attachment_points[side].x;
  quad.top_left.y = road_side->attachment_points[side].y;
  if(side == ROAD_SIDE_LEFT)
    {
      quad.top_left.x -= current_span;
    }
  quad.top_right.x = quad.top_left.x + current_span;
  quad.top_right.y = quad.top_left.y;
  quad.bottom_right.x = bounds.right;
  quad.bottom_right.y = bounds.top;
  quad.bottom_left.x = bounds.left;
  quad.bottom_left.y = bounds.top;

  if(side == ROAD_SIDE_LEFT)
    {
      previous_span = -previous_span;
      current_span = -current_span;
    }

  result = (CCB *)road_side;
  repeat_index = 0;
  while(repeat_index < repeat_count)
    {
      cel = append_unmapped_cel(
        (CCB *)select_cached_description_cel(
          (char **)cache->join_sources[1],
          (char *)lane->edge_resource_ids,
          ROAD_MARGIN_FILL_CEL_CHILD, side, (int *)&quad));
      map_fixed_road_bounds_to_cel(cel, &bounds);
      cel->ccb_XPos = bounds.left;

      result = append_horizontal_quad_cel(
        (CCB *)select_cached_description_cel(
          (char **)cache->join_sources[0],
          (char *)lane->edge_resource_ids,
          ROAD_MARGIN_PRIMARY_CEL_CHILD, side, (int *)&quad),
        &quad);

      bounds.right += previous_span;
      bounds.left += previous_span;
      quad.top_left.x += current_span;
      quad.top_right.x += current_span;
      quad.bottom_right.x += previous_span;
      quad.bottom_left.x += previous_span;
      repeat_index++;
    }

  return result;
}

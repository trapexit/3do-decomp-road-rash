#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"

enum TexturedRoadsideFillValue
{
  TEXTURED_ROADSIDE_PIXC = 0x1F
};

int
render_roadside_neighbor_overhang(RoadRenderSide *road_side,
                                  RoadSide        side);
int
render_solid_roadside_backfill(RoadRenderSide *road_side,
                               RoadSide        side);
CCB *
select_cached_side_cel(RoadRenderSide *road_side,
                       RoadSide        side,
                       const RoadQuad *quad);
void
trim_cel_to_lower_half(CCB *cel);

__inline
static
int
divide_roadside_span_by_two(int value)
{
  return (value + (int)((unsigned int)value >>
                        (ROAD_INTEGER_BIT_COUNT - 1))) >> 1;
}


int
render_textured_roadside_fill(RoadRenderSide *road_side,
                              RoadSide        side)
{
  RoadSegmentLaneRuntime *lane;
  RoadRenderSide *neighbor;
  RoadRenderPoint *lane_edge;
  RoadRenderPoint *shoulder_edge;
  RoadRenderPoint *near_point;
  RoadRenderPoint *far_point;
  RoadRenderPoint *attachment;
  RoadRenderPoint *neighbor_lane_edge;
  RoadRenderPoint *neighbor_shoulder_edge;
  RoadQuad quad;
  RoadFixed near_span;
  RoadFixed far_span;
  RoadFixed left_x;
  RoadFixed right_x;
  RoadFixed midpoint_a;
  RoadFixed midpoint_b;
  unsigned int surface_flags;
  CCB *cel;
  int top_left_x;
  int top_left_y;
  int top_right_x;
  int top_right_y;
  int bottom_right_x;
  int bottom_right_y;
  int bottom_left_x;
  int bottom_left_y;

  lane = road_side->lane;
  lane_edge = &road_side->lane_edges[side];
  shoulder_edge = &road_side->lane_edges[
    ROAD_SHOULDER_EDGE_LEFT + side];
  near_point = &road_side->mode_geometry[
    ROAD_MODE_GEOMETRY_LEFT_NEAR + side];
  far_point = &road_side->mode_geometry[
    ROAD_MODE_GEOMETRY_LEFT_FAR + side];
  attachment = &road_side->attachment_points[side];

  if(lane->surface_resource_ids[side] ==
     ROAD_SURFACE_RESOURCE_MISSING)
    {
      if(near_point->y != 0)
        {
          return near_point->y;
        }
      if((lane->surface_flags & ROAD_SURFACE_FLAG_DEPTH_ADJUSTED) != 0)
        {
          if(lane->previous_links[0]->geometry_mode !=
             ROAD_GEOMETRY_MODE_TEXTURED)
            {
              return lane->previous_links[0]->geometry_mode;
            }
        }
      render_roadside_neighbor_overhang(road_side, side);
      return render_solid_roadside_backfill(road_side, side);
    }

  render_roadside_neighbor_overhang(road_side, side);
  render_solid_roadside_backfill(road_side, side);

  neighbor = road_side->next_sides[side];
  neighbor_lane_edge = &neighbor->lane_edges[side];
  neighbor_shoulder_edge = &neighbor->lane_edges[
    ROAD_SHOULDER_EDGE_LEFT + side];
  surface_flags = lane->surface_flags;
  near_span = near_point->y;
  far_span = far_point->y;

  if((surface_flags & ROAD_SURFACE_FLAG_MARGIN_MASK) != 0)
    {
      if((surface_flags & ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
        {
          top_left_x = attachment->x;
          top_left_y = attachment->y - far_span;
          top_right_x = shoulder_edge->x;
          top_right_y = lane_edge->y - near_span;
          bottom_right_x = top_right_x;
          bottom_right_y = lane_edge->y;
          bottom_left_x = top_left_x;
          bottom_left_y = attachment->y;
        }
      else
        {
          top_left_x = neighbor_shoulder_edge->x;
          top_left_y = neighbor_lane_edge->y - far_span;
          top_right_x = attachment->x;
          top_right_y = attachment->y - near_span;
          bottom_right_x = top_right_x;
          bottom_right_y = attachment->y;
          bottom_left_x = top_left_x;
          bottom_left_y = neighbor_lane_edge->y;
        }
    }
  else
    {
      top_left_x = neighbor_shoulder_edge->x;
      top_left_y = neighbor_lane_edge->y - far_span;
      top_right_x = shoulder_edge->x;
      top_right_y = lane_edge->y - near_span;
      bottom_right_x = top_right_x;
      bottom_right_y = lane_edge->y;
      bottom_left_x = top_left_x;
      bottom_left_y = neighbor_lane_edge->y;
    }

  left_x = top_left_x;
  right_x = top_right_x;
  if(side == ROAD_SIDE_LEFT)
    {
      if(right_x >= left_x)
        {
          return left_x;
        }
    }
  else if(right_x <= left_x)
    {
      return left_x;
    }

  if((right_x > ROAD_SCREEN_RIGHT && left_x > ROAD_SCREEN_RIGHT) ||
     (right_x < 0 && left_x < 0))
    {
      return left_x;
    }

  bottom_right_y += ROAD_FIXED_ONE;
  bottom_left_y += ROAD_FIXED_ONE;

  quad.top_left.x = top_left_x;
  quad.top_left.y = top_left_y;
  quad.top_right.x = top_right_x;
  quad.top_right.y = top_right_y;
  quad.bottom_right.x = bottom_right_x;
  quad.bottom_right.y = bottom_right_y;
  quad.bottom_left.x = bottom_left_x;
  quad.bottom_left.y = bottom_left_y;

  cel = select_cached_side_cel(road_side, side, &quad);
  cel->ccb_PIXC = TEXTURED_ROADSIDE_PIXC;
  cel = append_unmapped_cel(cel);

  midpoint_a = divide_roadside_span_by_two(near_span) + top_right_y;
  midpoint_b = divide_roadside_span_by_two(far_span) + top_left_y;
  if(midpoint_a < midpoint_b)
    {
      midpoint_a = midpoint_b;
    }

  if(midpoint_a < 0)
    {
      trim_cel_to_lower_half(cel);
      top_left_y += divide_roadside_span_by_two(far_span);
      top_right_y += divide_roadside_span_by_two(near_span);
      quad.top_left.y = top_left_y;
      quad.top_right.y = top_right_y;
    }

  return (int)map_fixed_road_quad_to_cel(cel, &quad);
}

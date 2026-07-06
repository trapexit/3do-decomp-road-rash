#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"

enum RoadsideBackfillValue
{
  ROADSIDE_BACKFILL_COLOR = 0x1466,
  ROADSIDE_BACKFILL_TRIM_EIGHTH_SHIFT = 3,
  ROADSIDE_BACKFILL_TRIM_SIXTEENTH_SHIFT = 4
};

__inline
static
int
road_side_slot_result(const RoadRenderSide *road_side,
                      RoadSide              side)
{
  return (int)road_side + side * sizeof(RoadRenderPoint);
}


int
render_solid_roadside_backfill(RoadRenderSide *road_side,
                               RoadSide        side)
{
  RoadSegmentLaneRuntime *lane;
  RoadRenderSide *neighbor;
  RoadQuad quad;
  RoadRenderPoint *near_point;
  RoadRenderPoint *far_point;
  RoadRenderPoint *shoulder_point;
  RoadRenderPoint *neighbor_shoulder;
  RoadFixed near_span;
  RoadFixed far_span;
  RoadFixed trim;
  unsigned int surface_flags;
  int top_left_x;
  int top_left_y;
  int top_right_x;
  int top_right_y;
  int bottom_right_x;
  int bottom_right_y;
  int bottom_left_x;
  int bottom_left_y;

  lane = road_side->lane;
  neighbor = road_side->next_sides[side];
  near_point = &road_side->mode_geometry[
    ROAD_MODE_GEOMETRY_LEFT_NEAR + side];
  far_point = &road_side->mode_geometry[
    ROAD_MODE_GEOMETRY_LEFT_FAR + side];
  shoulder_point = &road_side->lane_edges[
    ROAD_SHOULDER_EDGE_LEFT + side];
  neighbor_shoulder = &neighbor->lane_edges[
    ROAD_SHOULDER_EDGE_LEFT + side];
  far_span = far_point->y;

  if(far_span == 0)
    {
      bottom_right_y = road_side->lane_edges[side].y;
      top_left_y = neighbor->lane_edges[side].y;
      if(bottom_right_y <= top_left_y)
        {
          return road_side_slot_result(road_side, side);
        }

      bottom_right_x = shoulder_point->x;
      bottom_left_x = bottom_right_x;
      top_left_x = neighbor_shoulder->x;
      if(side == ROAD_SIDE_LEFT)
        {
          if(bottom_right_x >= top_left_x || top_left_x <= 0)
            {
              return top_left_x;
            }
        }
      else
        {
          if(bottom_right_x <= top_left_x ||
             top_left_x >= ROAD_SCREEN_RIGHT)
            {
              return top_left_x;
            }
        }

      bottom_left_y = top_left_y;
      top_right_x = top_left_x;
      top_right_y = top_left_y + ROAD_FIXED_ONE;
      bottom_right_y += ROAD_FIXED_ONE;
      quad.top_left.x = top_left_x;
      quad.top_left.y = top_left_y;
      quad.top_right.x = top_right_x;
      quad.top_right.y = top_right_y;
      quad.bottom_right.x = bottom_right_x;
      quad.bottom_right.y = bottom_right_y;
      quad.bottom_left.x = bottom_left_x;
      quad.bottom_left.y = bottom_left_y;
      return (int)append_solid_quad_cel(
        (const DisplayQuad *)(const void *)&quad,
        ROADSIDE_BACKFILL_COLOR);
    }

  near_span = near_point->y;
  if(road_side->transition_geometry[side * ROAD_SIDE_COUNT] != 0)
    {
      trim = (near_span >> ROADSIDE_BACKFILL_TRIM_EIGHTH_SHIFT) +
             (near_span >> ROADSIDE_BACKFILL_TRIM_SIXTEENTH_SHIFT);
      near_span -= trim;
      trim = (far_span >> ROADSIDE_BACKFILL_TRIM_EIGHTH_SHIFT) +
             (far_span >> ROADSIDE_BACKFILL_TRIM_SIXTEENTH_SHIFT);
      far_span -= trim;
    }

  surface_flags = lane->surface_flags;
  if((surface_flags & ROAD_SURFACE_FLAG_MARGIN_MASK) != 0)
    {
      if((surface_flags & ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
        {
          top_right_x = shoulder_point->x;
          top_right_y = road_side->lane_edges[side].y - near_span;
          top_left_x = road_side->attachment_points[side].x;
          top_left_y = road_side->attachment_points[side].y - far_span;
        }
      else
        {
          top_right_x = road_side->attachment_points[side].x;
          top_right_y = road_side->attachment_points[side].y - near_span;
          top_left_x = neighbor_shoulder->x;
          top_left_y = neighbor->lane_edges[side].y - far_span;
        }
    }
  else
    {
      top_right_x = shoulder_point->x;
      top_right_y = road_side->lane_edges[side].y - near_span;
      top_left_x = neighbor_shoulder->x;
      top_left_y = neighbor->lane_edges[side].y - far_span;
    }

  if(top_right_y <= top_left_y)
    {
      return road_side_slot_result(road_side, side);
    }

  bottom_left_x = far_point->x + top_left_x;
  bottom_left_y = top_left_y;
  bottom_right_x = near_point->x + top_right_x;
  bottom_right_y = top_right_y;

  if(side == ROAD_SIDE_LEFT)
    {
      if(top_right_x >= top_left_x)
        {
          return top_left_x;
        }
    }
  else if(top_right_x <= top_left_x)
    {
      return top_right_x;
    }

  if((top_right_x > ROAD_SCREEN_RIGHT && top_left_x > ROAD_SCREEN_RIGHT) ||
     (top_right_x < 0 && top_left_x < 0))
    {
      return top_left_x;
    }

  quad.top_left.x = top_left_x;
  quad.top_left.y = top_left_y;
  quad.top_right.x = top_right_x;
  quad.top_right.y = top_right_y;
  quad.bottom_right.x = bottom_right_x;
  quad.bottom_right.y = bottom_right_y;
  quad.bottom_left.x = bottom_left_x;
  quad.bottom_left.y = bottom_left_y;

  return (int)append_solid_quad_cel(
    (const DisplayQuad *)(const void *)&quad, ROADSIDE_BACKFILL_COLOR);
}

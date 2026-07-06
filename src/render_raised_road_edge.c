#include "road_geometry.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "cel_dimension_resource.h"
#include "display_cel_runtime.h"
#include "integer_shift_runtime.h"
#include "road_surface_cel_selection.h"
#include "rw_semantic_data.h"

#include "stdlib.h"
#include "stdio.h"

int
absolute_int_difference(int first,
                        int second);

enum RaisedRoadEdgeValue
{
  ROAD_RAISED_EDGE_MINIMUM_DEPTH = 0x3C,
  ROAD_RAISED_SURFACE_MINIMUM_DEPTH = 0x40,
  ROAD_RAISED_SURFACE_BAND_COUNT = 6,
  ROAD_RAISED_SURFACE_RESOURCE_DEPTH_MASK = 3,
  ROAD_RAISED_SURFACE_RESOURCE_DEPTH_BIAS = 1,
  ROAD_RAISED_SURFACE_RESOURCE_DEPTH_SHIFT = 2,
  ROAD_RAISED_APPROXIMATION_HALF_SHIFT = 1,
  ROAD_RAISED_REPEAT_STEP_SHIFT = 2
};

enum RaisedRoadSurfaceBand
{
  ROAD_RAISED_BAND_LEFT_OUTER = 0,
  ROAD_RAISED_BAND_LEFT_SLOPE = 1,
  ROAD_RAISED_BAND_LEFT_CENTER = 2,
  ROAD_RAISED_BAND_RIGHT_CENTER = 3,
  ROAD_RAISED_BAND_RIGHT_SLOPE = 4,
  ROAD_RAISED_BAND_RIGHT_OUTER = 5
};

enum RaisedRoadSurfaceCel
{
  ROAD_RAISED_CEL_CENTER = 0,
  ROAD_RAISED_CEL_SLOPE = 1,
  ROAD_RAISED_CEL_OUTER = 2
};

enum RaisedRoadCurvePoint
{
  ROAD_RAISED_CURVE_LEFT_OUTER = 0,
  ROAD_RAISED_CURVE_LEFT_SLOPE = 1,
  ROAD_RAISED_CURVE_LEFT_CENTER = 2,
  ROAD_RAISED_CURVE_CENTER = 3,
  ROAD_RAISED_CURVE_RIGHT_SLOPE = 4,
  ROAD_RAISED_CURVE_RIGHT_OUTER = 5
};

void
render_road_edge_transition(RoadRenderSide *road_side,
                            RoadSide        side,
                            int             mode);
void
render_curved_road_surface(RoadRenderSide *road_side);

static
const
DisplayQuad *
display_quad_from_road_quad(const RoadQuad *quad)
{
  return (const DisplayQuad *)(const void *)quad;
}


static
void
copy_projected_road_point(RoadPoint             *destination,
                          const RoadRenderPoint *source)
{
  destination->x = source->x;
  destination->y = source->y;
}


static
int
road_side_has_curved_geometry(const RoadRenderSide *road_side)
{
  return road_side->lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED;
}


static
RoadFixed
approximate_road_distance(RoadFixed horizontal,
                          RoadFixed vertical,
                          int       absolute_vertical)
{
  RoadFixed smaller;

  horizontal = absolute_int_difference(horizontal, 0);
  if(absolute_vertical != 0)
    {
      vertical = absolute_int_difference(vertical, 0);
    }
  smaller = horizontal < vertical ? horizontal : vertical;
  return horizontal + vertical -
         (smaller >> ROAD_RAISED_APPROXIMATION_HALF_SHIFT);
}


static
int
append_raised_surface_band(RoadRenderSide *road_side,
                           int             resource_row,
                           int             cel_selector,
                           RoadFixed       width,
                           RoadFixed       height,
                           const RoadQuad *quad)
{
  CCB *source;

  source = (CCB *)road_side->texture_cache->
           procedural_sources[resource_row][cel_selector];
  if(source == 0)
    {
      return 0;
    }

  source = select_cel_for_dimensions(
    source, ROAD_FIXED_TO_INTEGER(width),
    ROAD_FIXED_TO_INTEGER(height));
  append_mapped_quad_cel(source, display_quad_from_road_quad(quad));
  return 1;
}


void
render_raised_road_surface(RoadRenderSide *road_side)
{
  RoadRenderSide *left_neighbor;
  RoadSegmentLaneRuntime *linked_lane;
  RoadQuad quad;
  int resource_row;
  RoadFixed width;
  RoadFixed height;
  int cel_selector;
  int should_emit;
  int band;

  render_road_edge_transition(road_side, ROAD_SIDE_LEFT, 0);
  render_road_edge_transition(road_side, ROAD_SIDE_RIGHT, 0);

  if(road_side->projection_depth >= ROAD_RAISED_SURFACE_MINIMUM_DEPTH)
    {
      left_neighbor = road_side->next_sides[ROAD_SIDE_LEFT];
      resource_row =
        (((road_side->owner_node->depth &
           ROAD_RAISED_SURFACE_RESOURCE_DEPTH_MASK) +
          ROAD_RAISED_SURFACE_RESOURCE_DEPTH_BIAS) >>
         ROAD_RAISED_SURFACE_RESOURCE_DEPTH_SHIFT);
      width = 0;
      height = 0;
      cel_selector = ROAD_RAISED_CEL_CENTER;

      for(band = ROAD_RAISED_BAND_LEFT_OUTER;
          band < ROAD_RAISED_SURFACE_BAND_COUNT; band++)
        {
          should_emit = 0;
          switch(band)
            {
            case ROAD_RAISED_BAND_LEFT_OUTER:
              copy_projected_road_point(
                &quad.top_left,
                &left_neighbor->curve_profile.
                points[ROAD_RAISED_CURVE_LEFT_OUTER]);
              copy_projected_road_point(
                &quad.top_right,
                &left_neighbor->curve_profile.
                points[ROAD_RAISED_CURVE_LEFT_SLOPE]);
              copy_projected_road_point(
                &quad.bottom_right,
                &road_side->curve_profile.
                points[ROAD_RAISED_CURVE_LEFT_SLOPE]);
              copy_projected_road_point(
                &quad.bottom_left,
                &road_side->curve_profile.
                points[ROAD_RAISED_CURVE_LEFT_OUTER]);
              quad.top_left.y += ROAD_FIXED_ONE;
              quad.bottom_left.y += ROAD_FIXED_ONE;
              quad.top_right.x += ROAD_FIXED_ONE;
              quad.top_left.x += ROAD_FIXED_ONE;
              height = quad.top_left.x - quad.bottom_left.x;
              if(height > 0 && quad.top_right.x >= 0)
                {
                  cel_selector = ROAD_RAISED_CEL_OUTER;
                  width = quad.bottom_left.y - quad.bottom_right.y;
                  quad.top_left.x =
                    ROAD_FIXED_FLOOR_QUARTER(quad.top_left.x);
                  quad.top_left.y =
                    ROAD_FIXED_FLOOR_QUARTER(quad.top_left.y);
                  quad.bottom_left.x =
                    ROAD_FIXED_FLOOR_QUARTER(quad.bottom_left.x);
                  quad.bottom_left.y =
                    ROAD_FIXED_FLOOR_QUARTER(quad.bottom_left.y);
                  should_emit = 1;
                }
              break;

            case ROAD_RAISED_BAND_LEFT_SLOPE:
              quad.top_left = quad.top_right;
              quad.top_left.y += ROAD_FIXED_ONE;
              quad.bottom_left = quad.bottom_right;
              quad.bottom_left.y += ROAD_FIXED_ONE;
              copy_projected_road_point(
                &quad.top_right,
                &left_neighbor->curve_profile.
                points[ROAD_RAISED_CURVE_LEFT_CENTER]);
              copy_projected_road_point(
                &quad.bottom_right,
                &road_side->curve_profile.
                points[ROAD_RAISED_CURVE_LEFT_CENTER]);
              quad.top_right.x += ROAD_FIXED_ONE;
              quad.top_left.x += ROAD_FIXED_ONE;
              if(quad.top_right.x > 0 && quad.top_left.y > 0)
                {
                  cel_selector = ROAD_RAISED_CEL_SLOPE;
                  width = approximate_road_distance(
                    quad.bottom_left.x - quad.bottom_right.x,
                    quad.bottom_left.y - quad.bottom_right.y, 0);
                  height = approximate_road_distance(
                    quad.bottom_left.x - quad.top_left.x,
                    quad.bottom_left.y - quad.top_left.y, 1);
                  should_emit = 1;
                }
              break;

            case ROAD_RAISED_BAND_LEFT_CENTER:
              quad.top_left = quad.top_right;
              quad.top_left.x -= ROAD_FIXED_ONE;
              quad.bottom_left = quad.bottom_right;
              quad.top_right.x =
                (quad.top_left.x +
                 left_neighbor->curve_profile.
                 points[ROAD_RAISED_CURVE_CENTER].x) >> 1;
              quad.bottom_right.x =
                (quad.bottom_left.x +
                 road_side->curve_profile.
                 points[ROAD_RAISED_CURVE_CENTER].x) >> 1;
              height = quad.top_right.y - quad.bottom_right.y;
              width = quad.bottom_right.x - quad.bottom_left.x;
              cel_selector = ROAD_RAISED_CEL_CENTER;
              if(height > 0 && quad.top_right.y > 0)
                {
                  should_emit = 1;
                }
              break;

            case ROAD_RAISED_BAND_RIGHT_CENTER:
              quad.top_left.x =
                left_neighbor->curve_profile.
                points[ROAD_RAISED_CURVE_CENTER].x;
              quad.bottom_left.x = road_side->curve_profile.
                                   points[ROAD_RAISED_CURVE_CENTER].x;
              quad.top_left.y += ROAD_FIXED_ONE;
              if(height > 0 && quad.top_right.y > 0)
                {
                  should_emit = 1;
                }
              break;

            case ROAD_RAISED_BAND_RIGHT_SLOPE:
              quad.top_right = quad.top_left;
              quad.top_right.x -= ROAD_FIXED_ONE;
              quad.bottom_right = quad.bottom_left;
              copy_projected_road_point(
                &quad.top_left,
                &left_neighbor->curve_profile.
                points[ROAD_RAISED_CURVE_RIGHT_SLOPE]);
              copy_projected_road_point(
                &quad.bottom_left,
                &road_side->curve_profile.
                points[ROAD_RAISED_CURVE_RIGHT_SLOPE]);
              quad.top_left.x -= ROAD_FIXED_ONE;
              if(quad.top_right.x < ROAD_SCREEN_RIGHT &&
                 quad.top_left.y > 0)
                {
                  cel_selector = ROAD_RAISED_CEL_SLOPE;
                  width = approximate_road_distance(
                    quad.bottom_left.x - quad.bottom_right.x,
                    quad.bottom_left.y - quad.bottom_right.y, 0);
                  height = approximate_road_distance(
                    quad.bottom_left.x - quad.top_left.x,
                    quad.bottom_left.y - quad.top_left.y, 1);
                  should_emit = 1;
                }
              break;

            case ROAD_RAISED_BAND_RIGHT_OUTER:
              quad.top_right = quad.top_left;
              quad.top_right.y -= ROAD_FIXED_ONE;
              quad.bottom_right = quad.bottom_left;
              quad.bottom_right.y -= ROAD_FIXED_ONE;
              copy_projected_road_point(
                &quad.top_left,
                &left_neighbor->curve_profile.
                points[ROAD_RAISED_CURVE_RIGHT_OUTER]);
              copy_projected_road_point(
                &quad.bottom_left,
                &road_side->curve_profile.
                points[ROAD_RAISED_CURVE_RIGHT_OUTER]);
              quad.top_left.x -= ROAD_FIXED_ONE;
              height = quad.bottom_left.x - quad.top_left.x;
              if(height > 0 && quad.top_right.x < ROAD_SCREEN_RIGHT)
                {
                  cel_selector = ROAD_RAISED_CEL_OUTER;
                  width = quad.bottom_left.y - quad.bottom_right.y;
                  should_emit = 1;
                }
              break;
            }

          if(should_emit != 0 &&
             append_raised_surface_band(
               road_side, resource_row, cel_selector, width, height,
               &quad) == 0)
            {
              return;
            }
        }
    }

  linked_lane = (RoadSegmentLaneRuntime *)road_side->lane->next_links[0];
  if(linked_lane->geometry_mode != ROAD_GEOMETRY_MODE_CURVED)
    {
      render_curved_road_surface(road_side);
    }
}


static
RoadFixed
scaled_cross_product_component(RoadFixed first,
                               RoadFixed second)
{
  return (first >> ROAD_FIXED_CROSS_PRODUCT_SHIFT) *
         (second >> ROAD_FIXED_CROSS_PRODUCT_SHIFT);
}


static
int
raised_edge_is_visible(const RoadQuad *quad,
                       RoadSide        side)
{
  RoadFixed first;
  RoadFixed second;

  if(side == ROAD_SIDE_RIGHT)
    {
      if(quad->bottom_left.x < quad->bottom_right.x)
        {
          return quad->bottom_left.x < ROAD_SCREEN_RIGHT;
        }

      first = scaled_cross_product_component(
        quad->top_left.x - quad->top_right.x,
        quad->bottom_right.y - quad->top_right.y);
      second = scaled_cross_product_component(
        quad->top_left.y - quad->top_right.y,
        quad->bottom_right.x - quad->top_right.x);
      if(first < second)
        {
          return 1;
        }

      first = scaled_cross_product_component(
        quad->bottom_right.x - quad->bottom_left.x,
        quad->top_left.y - quad->bottom_left.y);
      second = scaled_cross_product_component(
        quad->top_left.x - quad->bottom_left.x,
        quad->bottom_right.y - quad->bottom_left.y);
      return first < second;
    }

  if(quad->bottom_right.x < quad->bottom_left.x)
    {
      return quad->bottom_left.x > 0;
    }

  first = scaled_cross_product_component(
    quad->top_left.x - quad->top_right.x,
    quad->bottom_right.y - quad->top_right.y);
  second = scaled_cross_product_component(
    quad->top_left.y - quad->top_right.y,
    quad->bottom_right.x - quad->top_right.x);
  if(first > second)
    {
      return 1;
    }

  first = scaled_cross_product_component(
    quad->bottom_right.x - quad->bottom_left.x,
    quad->top_left.y - quad->bottom_left.y);
  second = scaled_cross_product_component(
    quad->top_left.x - quad->bottom_left.x,
    quad->bottom_right.y - quad->bottom_left.y);
  return first > second;
}


static
void
append_raised_edge_leading_quad(RoadRenderSide *road_side,
                                RoadSide        side,
                                RoadQuad       *quad)
{
  RoadFixed edge_step;
  CCB *source;
  CCB *packet;

  edge_step = side == ROAD_SIDE_RIGHT ? ROAD_FIXED_ONE : -ROAD_FIXED_ONE;
  quad->bottom_right.x += edge_step;
  quad->top_right.x += edge_step;
  source = select_road_strip_cel(road_side, side, quad, 0);
  packet = append_mapped_quad_cel(source,
                                  display_quad_from_road_quad(quad));
  packet->ccb_Flags &= ~(CCB_TWD | CCB_ACW | CCB_ACCW);
  if(side == ROAD_SIDE_RIGHT)
    {
      packet->ccb_Flags |= CCB_ACW;
    }
  else
    {
      packet->ccb_Flags |= CCB_ACCW;
    }
}


static
unsigned int
divide_rounding_up(unsigned int value,
                   unsigned int divisor)
{
  return value / divisor + (value % divisor != 0);
}


static
unsigned int
raised_edge_repeat_limit(const RoadQuad *quad,
                         RoadSide       side,
                         RoadFixed      road_step,
                         RoadFixed      neighbor_step)
{
  unsigned int road_distance;
  unsigned int neighbor_distance;
  unsigned int road_magnitude;
  unsigned int neighbor_magnitude;
  unsigned int road_count;
  unsigned int neighbor_count;

  if(side == ROAD_SIDE_LEFT)
    {
      road_distance = quad->bottom_left.x > 0
                        ? (unsigned int)quad->bottom_left.x : 0;
      neighbor_distance = quad->top_left.x > 0
                            ? (unsigned int)quad->top_left.x : 0;
      road_magnitude = 0U - (unsigned int)road_step;
      neighbor_magnitude = 0U - (unsigned int)neighbor_step;
    }
  else
    {
      road_distance = quad->bottom_left.x < ROAD_SCREEN_RIGHT
                        ? (unsigned int)ROAD_SCREEN_RIGHT -
                          (unsigned int)quad->bottom_left.x : 0;
      neighbor_distance = quad->top_left.x < ROAD_SCREEN_RIGHT
                            ? (unsigned int)ROAD_SCREEN_RIGHT -
                              (unsigned int)quad->top_left.x : 0;
      road_magnitude = (unsigned int)road_step;
      neighbor_magnitude = (unsigned int)neighbor_step;
    }

  road_count = divide_rounding_up(road_distance, road_magnitude);
  neighbor_count = divide_rounding_up(
    neighbor_distance, neighbor_magnitude);
  return road_count > neighbor_count ? road_count : neighbor_count;
}


static
void
append_repeated_raised_edge(RoadRenderSide *road_side,
                            RoadRenderSide *neighbor,
                            RoadSide        side,
                            RoadQuad       *quad)
{
  RoadFixed road_step;
  RoadFixed neighbor_step;
  unsigned int repeats_remaining;
  CCB *source;

  if(quad->top_right.y <= quad->top_left.y ||
     quad->top_right.y <= 0)
    {
      return;
    }

  road_step = shift_left_wrapping_32(
    road_side->projected_lane_unit, ROAD_RAISED_REPEAT_STEP_SHIFT);
  neighbor_step = shift_left_wrapping_32(
    neighbor->projected_lane_unit, ROAD_RAISED_REPEAT_STEP_SHIFT);
  if(side == ROAD_SIDE_LEFT)
    {
      road_step = negate_wrapping_32(road_step);
      neighbor_step = negate_wrapping_32(neighbor_step);
    }
  if(road_step == 0 || neighbor_step == 0 ||
     (side == ROAD_SIDE_LEFT &&
      (road_step >= 0 || neighbor_step >= 0)) ||
     (side == ROAD_SIDE_RIGHT &&
      (road_step <= 0 || neighbor_step <= 0)))
    {
      printf(
        "Raised road edge has non-progressing steps %d/%d on side %d; skipped\n",
        road_step, neighbor_step, side);
      return;
    }

  quad->bottom_left = quad->top_right;
  quad->top_right.x = add_wrapping_32(
    quad->top_left.x, neighbor_step);
  quad->top_right.y = quad->top_left.y;
  quad->bottom_right.x = add_wrapping_32(
    quad->bottom_left.x, road_step);
  quad->bottom_right.y = quad->bottom_left.y;
  repeats_remaining = raised_edge_repeat_limit(
    quad, side, road_step, neighbor_step);

  source = select_road_strip_cel(road_side, side, quad, 1);
  if(side == ROAD_SIDE_LEFT)
    {
      while(quad->top_left.x > 0 || quad->bottom_left.x > 0)
        {
          if(repeats_remaining == 0)
            {
              printf(
                "Raised road edge exceeded its left-side progress bound\n");
              break;
            }
          append_horizontal_quad_cel(source,
                                     display_quad_from_road_quad(quad));
          quad->top_left.x = add_wrapping_32(
            quad->top_left.x, neighbor_step);
          quad->top_right.x = add_wrapping_32(
            quad->top_right.x, neighbor_step);
          quad->bottom_right.x = add_wrapping_32(
            quad->bottom_right.x, road_step);
          quad->bottom_left.x = add_wrapping_32(
            quad->bottom_left.x, road_step);
          repeats_remaining--;
        }
    }
  else
    {
      while(quad->top_left.x < ROAD_SCREEN_RIGHT ||
            quad->bottom_left.x < ROAD_SCREEN_RIGHT)
        {
          if(repeats_remaining == 0)
            {
              printf(
                "Raised road edge exceeded its right-side progress bound\n");
              break;
            }
          append_horizontal_quad_cel(source,
                                     display_quad_from_road_quad(quad));
          quad->top_left.x = add_wrapping_32(
            quad->top_left.x, neighbor_step);
          quad->top_right.x = add_wrapping_32(
            quad->top_right.x, neighbor_step);
          quad->bottom_right.x = add_wrapping_32(
            quad->bottom_right.x, road_step);
          quad->bottom_left.x = add_wrapping_32(
            quad->bottom_left.x, road_step);
          repeats_remaining--;
        }
    }
}


void
render_raised_road_edge(RoadRenderSide *road_side,
                        RoadSide        side)
{
  RoadRenderSide *neighbor;
  RoadRenderPoint *road_far_edge;
  RoadRenderPoint *neighbor_far_edge;
  RoadQuad quad;
  int both_curved;

  both_curved = road_side_has_curved_geometry(road_side) &&
                road_side_has_curved_geometry(
    road_side->next_sides[ROAD_SIDE_LEFT]);

  if(road_side->projection_depth > ROAD_RAISED_EDGE_MINIMUM_DEPTH)
    {
      if(road_surface_render_globals.disabled != 0 && both_curved != 0)
        {
          render_raised_road_surface(road_side);
          return;
        }

      neighbor = road_side->next_sides[side];
      road_far_edge =
        &road_side->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR + side];
      neighbor_far_edge =
        &neighbor->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR + side];

      if(both_curved != 0)
        {
          copy_projected_road_point(&quad.top_left, neighbor_far_edge);
          copy_projected_road_point(&quad.top_right, road_far_edge);
          copy_projected_road_point(&quad.bottom_right,
                                    &road_side->curve_control);
          copy_projected_road_point(&quad.bottom_left,
                                    &neighbor->curve_control);
        }
      else
        {
          if(neighbor->lane->geometry_mode ==
             ROAD_GEOMETRY_MODE_PROFILE)
            {
              copy_projected_road_point(
                &quad.top_left,
                &neighbor->mode_geometry[
                  ROAD_MODE_GEOMETRY_LEFT_NEAR + side]);
              copy_projected_road_point(
                &quad.bottom_left,
                &neighbor->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side]);
            }
          else
            {
              copy_projected_road_point(&quad.top_left,
                                        neighbor_far_edge);
              copy_projected_road_point(
                &quad.bottom_left,
                &neighbor->mode_geometry[
                  ROAD_MODE_GEOMETRY_LEFT_NEAR + side]);
            }
          copy_projected_road_point(&quad.top_right, road_far_edge);
          copy_projected_road_point(
            &quad.bottom_right,
            &road_side->mode_geometry[
              ROAD_MODE_GEOMETRY_LEFT_NEAR + side]);
        }

      quad.bottom_right.y += ROAD_FIXED_ONE;
      quad.bottom_left.y += ROAD_FIXED_ONE;

      if(raised_edge_is_visible(&quad, side) != 0)
        {
          append_raised_edge_leading_quad(road_side, side, &quad);
        }
      append_repeated_raised_edge(road_side, neighbor, side, &quad);
    }

  if(road_side_has_curved_geometry(road_side) &&
     road_side_has_curved_geometry(
       road_side->next_sides[ROAD_SIDE_LEFT]))
    {
      render_raised_road_surface(road_side);
    }
}

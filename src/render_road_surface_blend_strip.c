#include "road_geometry.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "rw_semantic_data.h"

enum RoadSurfaceBlendStripValue
{
  ROAD_BLEND_HEIGHT_THRESHOLD_COUNT = 2,
  ROAD_BLEND_STYLE_VARIANT_MASK = 7,
  ROAD_BLEND_CELS_PER_HEIGHT = 2
};

int
select_repeated_roadside_strip_count(RoadRenderSide *road_side);

int
render_road_surface_blend_strip(RoadRenderSide *road_side,
                                RoadSide        side,
                                int             surface_flags)
{
  RoadRenderSide *neighbor;
  RoadSegmentLaneRuntime *lane;
  RoadQuad quad;
  RoadFixed source_step;
  RoadFixed neighbor_step;
  int height;
  int height_index;
  int style_variant;
  int cel_index;
  CCB *source;
  int strip_count;
  int continue_rendering;

  (void)surface_flags;

  neighbor = road_side->next_sides[side];
  source_step =
    road_side->projection_scale << ROAD_8_8_FRACTION_BITS;
  neighbor_step =
    neighbor->projection_scale << ROAD_8_8_FRACTION_BITS;
  if(side == ROAD_SIDE_LEFT)
    {
      source_step = -source_step;
      neighbor_step = -neighbor_step;
    }

  quad.top_left.x = neighbor->lane_edges[side].x;
  quad.top_left.y =
    road_side->connection_geometry[ROAD_CONNECTION_CENTER_Y];
  quad.top_right.x = quad.top_left.x + neighbor_step;
  quad.top_right.y = 0;
  quad.bottom_right.x = road_side->lane_edges[side].x + source_step;
  quad.bottom_right.y = 0;
  quad.bottom_left.x = road_side->lane_edges[side].x;
  quad.bottom_left.y =
    road_side->connection_geometry[ROAD_CONNECTION_SOURCE_CENTER_Y];

  height = ROAD_FIXED_TO_INTEGER(
    quad.bottom_left.y - quad.top_left.y);
  height_index = 0;
  while(height_index < ROAD_BLEND_HEIGHT_THRESHOLD_COUNT &&
        road_surface_thresholds[height_index] > height)
    {
      height_index++;
    }

  lane = road_side->lane;
  style_variant = lane->surface_flags & ROAD_BLEND_STYLE_VARIANT_MASK;
  if(style_variant != 0)
    {
      style_variant--;
    }
  cel_index =
    height_index * ROAD_BLEND_CELS_PER_HEIGHT + style_variant;

  if(road_quad_is_horizontally_visible(
       road_side, &quad, side == ROAD_SIDE_LEFT) != 0)
    {
      source = (CCB *)road_surface_cel_table_a[cel_index];
      append_horizontal_quad_cel(
        source, (const DisplayQuad *)(const void *)&quad);
    }

  source = (CCB *)road_surface_cel_table_b[cel_index];
  strip_count = select_repeated_roadside_strip_count(road_side) - 1;
  do
    {
      quad.top_left.x += neighbor_step;
      quad.top_right.x += neighbor_step;
      quad.bottom_right.x += source_step;
      quad.bottom_left.x += source_step;

      if(road_quad_is_horizontally_visible(
           road_side, &quad, side == ROAD_SIDE_LEFT) != 0)
        {
          append_horizontal_quad_cel(
            source, (const DisplayQuad *)(const void *)&quad);
        }

      continue_rendering = strip_count;
      strip_count--;
      if(continue_rendering <= 0)
        {
          break;
        }

      if(side == ROAD_SIDE_LEFT)
        {
          continue_rendering = quad.top_left.x > 0;
        }
      else
        {
          continue_rendering = quad.top_left.x < ROAD_SCREEN_RIGHT;
        }
    } while(continue_rendering != 0);

  return continue_rendering;
}

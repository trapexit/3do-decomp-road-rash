#include "bss_early_data.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RoadCenterClipValue
{
  ROAD_CENTER_CLIP_DEPTH = ROAD_SUBDIVISION_MIN_DEPTH - 1,
  ROAD_CENTER_CLIP_DEPTH_BIAS =
    ROAD_PROJECTION_DEPTH_STEP - ROAD_CENTER_CLIP_DEPTH,
  ROAD_CENTER_CLIP_INTERPOLATION_SHIFT = ROAD_8_8_FRACTION_BITS
};

int
clip_road_quad_to_near_depth(RoadRenderSide *road_side,
                             RoadQuad       *quad,
                             RoadFixed       source_offset,
                             RoadFixed       neighbor_offset,
                             RoadFixed      *projected_width,
                             RoadFixed       source_width)
{
  RoadRenderSide *neighbor;
  int interpolation;
  int scale;
  RoadFixed world_x;
  RoadFixed world_y;

  interpolation = 0;
  if(road_side->projection_depth <= ROAD_CENTER_CLIP_DEPTH)
    {
      interpolation = road_side->projection_depth +
                      ROAD_CENTER_CLIP_DEPTH_BIAS;
      world_x = road_side->world_x + source_offset;
      neighbor = road_side->next_sides[ROAD_SIDE_LEFT];
      scale = ROAD_PROJECTION_DEPTH_STEP - interpolation;
      world_x += ((neighbor->world_x + neighbor_offset - world_x) *
                  scale) >> ROAD_CENTER_CLIP_INTERPOLATION_SHIFT;

      world_y = road_side->world_y;
      world_y += ((neighbor->world_y - world_y) * scale) >>
                 ROAD_CENTER_CLIP_INTERPOLATION_SHIFT;

      *projected_width =
        source_width * road_renderer_state.reciprocal_table[
          ROAD_CENTER_CLIP_DEPTH >> 1];
      project_road_world_point(world_x, world_y, ROAD_CENTER_CLIP_DEPTH,
                               &quad->bottom_left);

      quad->bottom_right.x = quad->bottom_left.x + *projected_width;
      quad->bottom_right.y = quad->bottom_left.y;
    }

  return interpolation;
}

#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

#include "stdio.h"

enum RoadBackgroundRenderConstant
{
  ROAD_BACKGROUND_MANAGER_NODE_INDEX = 32,
  ROAD_BACKGROUND_PRIMARY_LAYER_INDEX = 0,
  ROAD_BACKGROUND_OVERLAY_LAYER_INDEX = 3,
  ROAD_BACKGROUND_BOTTOM_LIMIT = 0x5E,
  ROAD_BACKGROUND_VIEW_WIDTH = 300,
  ROAD_BACKGROUND_DIMENSION_MASK = 0x1FF,
  ROAD_BACKGROUND_DIMENSION_EXPONENT_MASK = 0x1F,
  ROAD_BACKGROUND_SCROLL_SHIFT = 16,
  ROAD_BACKGROUND_MAXIMUM_TILE_COUNT =
    ROAD_BACKGROUND_VIEW_WIDTH + 1
};

#define ROAD_BACKGROUND_MAXIMUM_SIGNED_SPAN 0x7FFFFFFFU

int
draw_clipped_background_cel(const CCB              *source,
                            int                     x,
                            int                     y,
                            const DisplayRectangle *clip_bounds);
void
render_road_particles(const DisplayRectangle    *clip_bounds,
                      const RendererMotionState *motion);
int
render_road_horizon_bands(int     *road_node_words,
                          RoadSide side);
int
set_cel_shading_depth(int depth);

static
unsigned int
decode_background_dimension(int field)
{
  if(field >= 0)
    {
      return (unsigned int)field & ROAD_BACKGROUND_DIMENSION_MASK;
    }
  return 1U << (field & ROAD_BACKGROUND_DIMENSION_EXPONENT_MASK);
}


static
int
first_tiled_background_position(int          remainder,
                                unsigned int span)
{
  int start;

  start = -remainder;
  if(start > 0)
    {
      start -= span;
    }
  return start;
}


static
void
render_tiled_background_layer(const CCB              *layer,
                              int                     layer_index,
                              int                     y,
                              int                     scroll,
                              const DisplayRectangle *clip_bounds)
{
  unsigned int span;
  unsigned int tiles_remaining;
  int position;
  SignedDivisionResult division;

  span = decode_background_dimension(layer->ccb_Width);
  if(span == 0 || span > ROAD_BACKGROUND_MAXIMUM_SIGNED_SPAN)
    {
      printf(
        "Road background layer %d has invalid decoded width %u; skipped\n",
        layer_index, span);
      return;
    }

  division = signed_divide_with_remainder(
    span, (unsigned int)scroll);
  position = first_tiled_background_position(
    division.remainder, span);
  tiles_remaining = ROAD_BACKGROUND_MAXIMUM_TILE_COUNT;
  while(position < ROAD_BACKGROUND_VIEW_WIDTH &&
        tiles_remaining != 0)
    {
      draw_clipped_background_cel(
        layer, position, y, clip_bounds);
      position += (int)span;
      tiles_remaining--;
    }
  if(position < ROAD_BACKGROUND_VIEW_WIDTH)
    {
      printf(
        "Road background layer %d did not make bounded tiling progress\n",
        layer_index);
    }
}


void
render_road_background_layers(DisplayRectangle *clip_bounds)
{
  RoadRenderNode *manager;
  RoadSegmentLaneRuntime *lane;
  CCB *primary_layer;
  CCB *overlay_layer;
  unsigned int primary_height;
  unsigned int overlay_height;
  int scroll;
  RoadSide horizon_band_side;

  manager = &gRoadRenderNodes[ROAD_BACKGROUND_MANAGER_NODE_INDEX];
  lane = manager->left_side->lane;
  horizon_band_side = -1;

  if(lane->geometry_mode == ROAD_GEOMETRY_MODE_EDGE ||
     lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
    {
      if(lane->edge_shapes[ROAD_SIDE_LEFT].height <= 0)
        {
          horizon_band_side = ROAD_SIDE_LEFT;
        }
      else if(lane->edge_shapes[ROAD_SIDE_RIGHT].height <= 0)
        {
          horizon_band_side = ROAD_SIDE_RIGHT;
        }

      if(horizon_band_side >= 0 &&
         clip_bounds->bottom > ROAD_BACKGROUND_BOTTOM_LIMIT)
        {
          clip_bounds->bottom = ROAD_BACKGROUND_BOTTOM_LIMIT;
        }
    }

  primary_layer =
    road_renderer_state
    .road_layers[ROAD_BACKGROUND_PRIMARY_LAYER_INDEX];
  if(primary_layer != 0)
    {
      scroll = road_renderer_state.motion.road_scroll_phase >>
               ROAD_BACKGROUND_SCROLL_SHIFT;
      render_tiled_background_layer(
        primary_layer, ROAD_BACKGROUND_PRIMARY_LAYER_INDEX, 0, scroll,
        clip_bounds);

      render_road_particles(clip_bounds, &road_renderer_state.motion);

      overlay_layer =
        road_renderer_state
        .road_layers[ROAD_BACKGROUND_OVERLAY_LAYER_INDEX];
      if(overlay_layer != 0)
        {
          primary_height =
            decode_background_dimension(primary_layer->ccb_Height);
          overlay_height =
            decode_background_dimension(overlay_layer->ccb_Height);
          render_tiled_background_layer(
            overlay_layer, ROAD_BACKGROUND_OVERLAY_LAYER_INDEX,
            (int)primary_height - (int)overlay_height, scroll,
            clip_bounds);
        }
    }

  set_cel_shading_depth(0);
  if(horizon_band_side >= 0)
    {
      render_road_horizon_bands(
        (int *)manager, horizon_band_side);
    }
}

#undef ROAD_BACKGROUND_MAXIMUM_SIGNED_SPAN

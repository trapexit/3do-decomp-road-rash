#include "road_surface_cel_selection.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "rw_semantic_data.h"

enum SubdividedRoadSurfaceValue
{
  SUBDIVIDED_ROAD_BAND_COUNT = 4,
  SUBDIVIDED_ROAD_MINIMUM_DEPTH = 0xD2,
  SUBDIVIDED_ROAD_REVERSED_CORNER_MASK = 3,
  SUBDIVIDED_ROAD_DIRECT_PIXC = 0x18,
  SUBDIVIDED_ROAD_DEFAULT_PIXC = 0x1F001F00,
  SUBDIVIDED_ROAD_CEL_LITERAL_SHIFT = 23
};

/* The CEL source of a band is fixed apart from the strip variant: the
   dynamic-cel flag, the surface flags and the style row cannot change between
   the strips of one side, so they are resolved once per band.  A fixed CEL
   uses `cel`; a per-variant table row uses `row`. */
typedef struct SubdividedStripCelSource
{
  void **row;
  CCB   *cel;
} SubdividedStripCelSource;

__inline
static
int
divide_road_fixed_by_256(int value)
{
  if(value < 0)
    {
      value += ROAD_8_8_FRACTION_MASK;
    }
  return value >> ROAD_8_8_FRACTION_BITS;
}


__inline
static
int
divide_road_value_by_four(int value)
{
  if(value < 0)
    {
      value += ROAD_SUBDIVISION_QUARTER_ROUNDING_BIAS;
    }
  return value >> ROAD_SUBDIVISION_QUARTER_SHIFT;
}


__inline
static
int
decode_cel_height(int packed_height)
{
  if(packed_height >= 0)
    {
      return (int)(((unsigned int)packed_height <<
                    SUBDIVIDED_ROAD_CEL_LITERAL_SHIFT) >>
                   SUBDIVIDED_ROAD_CEL_LITERAL_SHIFT);
    }
  return 1 << (packed_height & DISPLAY_CEL_SOURCE_SHIFT_MASK);
}


__inline
static
int
subdivided_road_quad_is_horizontally_visible(const RoadQuad *quad)
{
  return (quad->top_left.x < ROAD_SCREEN_RIGHT &&
          quad->top_right.x > 0) ||
         (quad->bottom_left.x < ROAD_SCREEN_RIGHT &&
          quad->bottom_right.x > 0);
}


__inline
static
void
resolve_subdivided_strip_cel_source(RoadRenderSide           *road_side,
                                    int                       surface_style,
                                    SubdividedStripCelSource *strip_cel)
{
  RoadSegmentLaneRuntime *lane;
  unsigned int surface_flags;

  lane = road_side->lane;
  surface_flags = lane->surface_flags;
  if((lane->topology_flags & ROAD_TOPOLOGY_DYNAMIC_CEL_FLAG) != 0 &&
     road_surface_dynamic_cels[0] != 0)
    {
      strip_cel->cel = (CCB *)road_surface_dynamic_cels[surface_style];
      strip_cel->row = 0;
    }
  else if(surface_flags == 0 ||
          (surface_flags & ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG) != 0)
    {
      strip_cel->cel = 0;
      strip_cel->row = gRoadSurfaceCelVariants[surface_style];
    }
  else if((surface_flags &
           (ROAD_SURFACE_FLAG_REVERSED_MARGIN |
            ROAD_SURFACE_FLAG_DEPTH_ADJUSTED)) != 0)
    {
      strip_cel->cel = 0;
      strip_cel->row = road_surface_cel_variants[surface_style];
    }
  else if((surface_flags & ROAD_SURFACE_FLAG_BLEND_TRANSITION) != 0)
    {
      strip_cel->cel = (CCB *)road_surface_palette_variants[surface_style];
      strip_cel->row = 0;
    }
  else
    {
      strip_cel->cel = 0;
      strip_cel->row = gRoadSurfaceCelVariants[surface_style];
    }
}


__inline
static
void
emit_subdivided_surface_strip(const SubdividedStripCelSource *strip_cel,
                              int                             left_width,
                              int                             right_width,
                              int                             strip_index,
                              int                             pixel_control,
                              int                             source_row,
                              RoadQuad                       *quad)
{
  CCB *source;
  CCB *packet;
  int cel_height;

  if(strip_cel->cel != 0)
    {
      source = strip_cel->cel;
    }
  else
    {
      source = (CCB *)strip_cel->row[select_strip_variant(
        left_width, right_width, strip_index)];
    }
  source->ccb_PIXC = pixel_control;

  packet = append_unmapped_cel(source);
  cel_height = decode_cel_height(packet->ccb_Height);
  packet->ccb_SourcePtr = select_cel_source_row(
    packet,
    divide_road_value_by_four(source_row * cel_height));
  packet->ccb_Height -= ROAD_SUBDIVISION_CEL_HEIGHT_ADJUST;
  packet->ccb_PRE0 =
    (packet->ccb_PRE0 & ~PRE0_VCNT_MASK) |
    ROAD_PRE0_SHADE_BITS(divide_road_value_by_four(cel_height));

  map_cel_to_horizontal_quad(packet, quad);
}


int
render_subdivided_road_surface_strips(RoadRenderSide *road_side)
{
  RoadRenderSide *neighbor;
  RoadSegmentLaneRuntime *lane;
  int *reciprocal_table;
  RoadQuad quad;
  int right_width;
  int left_width;
  int strip_count;
  int depth;
  int band_index;
  int neighbor_span;
  int current_x;
  int current_y;
  int projected_x;
  int projected_y;
  int projected_span;
  int surface_style;
  int pixel_control;
  int reversed_corner_mask;
  int source_row;
  int depth_index;
  int interpolated_x;
  int interpolated_y;
  SubdividedStripCelSource strip_cel;

  lane = road_side->lane;
  neighbor = road_side->next_sides[ROAD_SIDE_LEFT];
  right_width = lane->right_width;
  left_width = lane->left_width;
  strip_count = lane->left_width_cells + lane->right_width_cells - 1;

  neighbor_span = neighbor->projected_lane_unit;
  current_x = road_side->connection_geometry[ROAD_CONNECTION_LEFT_X];
  current_y = ROAD_FIXED_FLOOR_QUARTER(
    road_side->connection_geometry[ROAD_CONNECTION_CENTER_Y]);
  depth = neighbor->projection_depth - ROAD_SUBDIVISION_DEPTH_STEP;

  if((lane->surface_flags & ROAD_SURFACE_FLAG_DEPTH_ADJUSTED) != 0)
    {
      reversed_corner_mask = SUBDIVIDED_ROAD_REVERSED_CORNER_MASK;
    }
  else
    {
      reversed_corner_mask = 0;
    }

  if(lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED &&
     neighbor->lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
    {
      pixel_control = SUBDIVIDED_ROAD_DIRECT_PIXC;
    }
  else
    {
      pixel_control = SUBDIVIDED_ROAD_DEFAULT_PIXC;
    }

  reciprocal_table = road_renderer_state.reciprocal_table;
  band_index = 0;
  while(band_index < SUBDIVIDED_ROAD_BAND_COUNT)
    {
      if(depth < SUBDIVIDED_ROAD_MINIMUM_DEPTH ||
         current_y >= ROAD_SCREEN_ROAD_BOTTOM)
        {
          return (int)road_side;
        }

      depth_index = depth >> 1;
      projected_span =
        reciprocal_table[depth_index] * ROAD_LANE_SAMPLE_SPACING;

      interpolated_x = neighbor->world_x - road_side->world_x;
      interpolated_x *= depth - road_side->projection_depth;
      projected_x = road_side->world_x +
                    divide_road_fixed_by_256(interpolated_x);
      projected_x += lane->connected_left_bound;

      interpolated_y = neighbor->world_y - road_side->world_y;
      interpolated_y *= depth - road_side->projection_depth;
      projected_y = road_side->world_y +
                    divide_road_fixed_by_256(interpolated_y);

      projected_x = reciprocal_table[depth_index] *
                    (projected_x -
                     road_renderer_state.motion.projection_origin_x) +
                    ROAD_PROJECTION_SCREEN_X_CENTER;
      projected_y = reciprocal_table[depth_index] *
                    (road_renderer_state.motion.projection_origin_y -
                     projected_y) +
                    ROAD_PROJECTION_SCREEN_Y_ORIGIN;
      projected_y = ROAD_FIXED_FLOOR_QUARTER(projected_y);

      if(reversed_corner_mask == 0)
        {
          quad.top_left.x = current_x;
          quad.top_left.y = current_y;
          quad.top_right.x = current_x + neighbor_span;
          quad.top_right.y = current_y;
          quad.bottom_right.x = projected_x + projected_span;
          quad.bottom_right.y = projected_y;
          quad.bottom_left.x = projected_x;
          quad.bottom_left.y = projected_y;
          source_row = band_index;
        }
      else
        {
          quad.top_left.x = projected_x;
          quad.top_left.y = projected_y;
          quad.top_right.x = projected_x + projected_span;
          quad.top_right.y = projected_y;
          quad.bottom_right.x = current_x + neighbor_span;
          quad.bottom_right.y = current_y;
          quad.bottom_left.x = current_x;
          quad.bottom_left.y = current_y;
          source_row = band_index ^ reversed_corner_mask;
        }

      surface_style = select_road_surface_style(
        road_side, &quad,
        SUBDIVIDED_ROAD_BAND_COUNT);
      resolve_subdivided_strip_cel_source(
        road_side, surface_style, &strip_cel);

      quad.top_left.x -= ROAD_TRANSITION_EDGE_INSET;
      quad.bottom_left.x -= ROAD_TRANSITION_EDGE_INSET;

      depth_index = 0;
      while(depth_index < strip_count)
        {
          if(subdivided_road_quad_is_horizontally_visible(&quad))
            {
              emit_subdivided_surface_strip(
                &strip_cel, left_width, right_width,
                depth_index, pixel_control, source_row, &quad);
            }

          quad.bottom_left = quad.bottom_right;
          quad.top_left = quad.top_right;
          if(reversed_corner_mask != 0)
            {
              quad.bottom_right.x += neighbor_span;
              quad.top_right.x += projected_span;
            }
          else
            {
              quad.bottom_right.x += projected_span;
              quad.top_right.x += neighbor_span;
            }
          depth_index++;
        }

      if(strip_count >= 0)
        {
          quad.top_right.x += ROAD_TRANSITION_EDGE_INSET;
          quad.bottom_right.x += ROAD_TRANSITION_EDGE_INSET;
          if(subdivided_road_quad_is_horizontally_visible(&quad))
            {
              emit_subdivided_surface_strip(
                &strip_cel, left_width, right_width,
                depth_index, pixel_control, source_row, &quad);
            }
        }

      neighbor_span = projected_span;
      current_x = projected_x;
      current_y = projected_y;
      depth -= ROAD_SUBDIVISION_DEPTH_STEP;
      band_index++;
    }

  return (int)road_side;
}

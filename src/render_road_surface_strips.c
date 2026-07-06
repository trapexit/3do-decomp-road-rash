#include "road_surface_cel_selection.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"

enum RoadSurfaceStripRenderValue
{
  ROAD_SURFACE_NEAR_DEPTH = 0x2D0,
  ROAD_SURFACE_NEAR_DEPTH_COUNT = 2,
  ROAD_SURFACE_DIRECT_PIXC = 0x18,
  ROAD_SURFACE_DEFAULT_PIXC = 0x1F001F00
};

__inline
static
int
road_surface_quad_is_visible(const RoadQuad *quad)
{
  return (quad->top_left.x < ROAD_SCREEN_RIGHT &&
          quad->top_right.x > 0) ||
         (quad->bottom_left.x < ROAD_SCREEN_RIGHT &&
          quad->bottom_right.x > 0);
}


/* Emits one road surface strip from an already resolved CEL.  Shared by the
   specialised and the general strip loops so they cannot drift apart. */
static
__inline
void
emit_road_surface_strip(const RoadQuad *quad,
                        CCB            *source,
                        int             pixel_control)
{
  if(road_surface_quad_is_visible(quad))
    {
      source->ccb_PIXC = pixel_control;
      append_horizontal_quad_cel(
        source, (const DisplayQuad *)(const void *)quad);
    }
}


int
render_road_surface_strips(RoadRenderSide *road_side)
{
  RoadRenderSide *neighbor;
  RoadSegmentLaneRuntime *lane;
  RoadQuad quad;
  RoadFixed top_span;
  RoadFixed bottom_span;
  int strip_count;
  int surface_style;
  int strip_index;
  int pixel_control;
  int left_width;
  int right_width;
  unsigned int surface_flags;
  void **style_cels;
  void **variant_cels;
  void **style_row = 0;
  CCB *style_cel = 0;
  CCB *source;

  if(road_side->active == 0)
    {
      return (int)road_side;
    }

  if(road_side->projection_depth <
     ROAD_SURFACE_NEAR_DEPTH + ROAD_SURFACE_NEAR_DEPTH_COUNT)
    {
      return render_subdivided_road_surface_strips(road_side);
    }

  surface_style = -1;
  lane = road_side->lane;
  neighbor = road_side->next_sides[ROAD_SIDE_LEFT];
  bottom_span = road_side->projected_lane_unit;
  top_span = neighbor->projected_lane_unit;
  strip_count = lane->left_width_cells + lane->right_width_cells - 1;
  left_width = lane->left_width;
  right_width = lane->right_width;

  /* Which cel set feeds the strips depends only on the lane's topology and
     surface flags, neither of which can change between strips of a side.
     Resolve it once: re-deriving it per strip cost a lane load, a topology
     byte load and the surface-flag branch chain on every strip. */
  style_cels = 0;
  variant_cels = 0;
  if((lane->topology_flags & ROAD_TOPOLOGY_DYNAMIC_CEL_FLAG) != 0 &&
     road_surface_dynamic_cels[0] != 0)
    {
      style_cels = road_surface_dynamic_cels;
    }
  else
    {
      surface_flags = lane->surface_flags;
      if(surface_flags == 0 ||
         (surface_flags & ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG) != 0)
        {
          variant_cels = (void **)gRoadSurfaceCelVariants;
        }
      else if((surface_flags &
               (ROAD_SURFACE_FLAG_REVERSED_MARGIN |
                ROAD_SURFACE_FLAG_DEPTH_ADJUSTED)) != 0)
        {
          variant_cels = (void **)road_surface_cel_variants;
        }
      else if((surface_flags & ROAD_SURFACE_FLAG_BLEND_TRANSITION) != 0)
        {
          style_cels = (void **)road_surface_palette_variants;
        }
      else
        {
          variant_cels = (void **)gRoadSurfaceCelVariants;
        }
    }

  quad.top_left.x =
    road_side->connection_geometry[ROAD_CONNECTION_LEFT_X];
  quad.top_left.y = ROAD_FIXED_FLOOR_QUARTER(
    road_side->connection_geometry[ROAD_CONNECTION_CENTER_Y]);
  quad.top_right.x = quad.top_left.x + top_span;
  quad.top_right.y = quad.top_left.y;

  quad.bottom_left.x =
    road_side->connection_geometry[ROAD_CONNECTION_SOURCE_LEFT_X];
  quad.bottom_left.y = ROAD_FIXED_FLOOR_QUARTER(
    road_side->connection_geometry[ROAD_CONNECTION_SOURCE_CENTER_Y]);
  quad.bottom_right.x = quad.bottom_left.x + bottom_span;
  quad.bottom_right.y = quad.bottom_left.y;

  if((lane->surface_flags & ROAD_SURFACE_FLAG_DEPTH_ADJUSTED) != 0)
    {
      RoadPoint saved_top_left;
      RoadPoint saved_top_right;
      RoadFixed saved_span;

      saved_top_left = quad.top_left;
      quad.top_left = quad.bottom_left;
      quad.bottom_left = saved_top_left;

      saved_top_right = quad.top_right;
      quad.top_right = quad.bottom_right;
      quad.bottom_right = saved_top_right;

      saved_span = top_span;
      top_span = bottom_span;
      bottom_span = saved_span;
    }

  quad.top_left.x -= ROAD_TRANSITION_EDGE_INSET;
  quad.bottom_left.x -= ROAD_TRANSITION_EDGE_INSET;

  if(lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED &&
     neighbor->lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
    {
      pixel_control = ROAD_SURFACE_DIRECT_PIXC;
    }
  else
    {
      pixel_control = ROAD_SURFACE_DEFAULT_PIXC;
    }

  /* The dominant case (no dynamic cels, no geometry override, no blend or
     reversed margin) draws straight out of the style-by-variant grid.  The
     table is then a constant address, so the strip loop needs no per-strip
     table dispatch or reload.  The style row is resolved once before the
     loop: re-deriving it per strip kept the surface style and the grid base
     alive across every iteration. */
  if(style_cels == 0 &&
     variant_cels == (void **)gRoadSurfaceCelVariants)
    {
      strip_index = 0;
      if(strip_count > 0)
        {
          surface_style = select_road_surface_style(
            road_side, &quad, 0);
          style_row = gRoadSurfaceCelVariants[surface_style];
          while(strip_index < strip_count)
            {
              source = (CCB *)style_row[select_strip_variant(
                left_width, right_width, strip_index)];
              if(source == 0)
                {
                  source = display_runtime_globals.fallback_ccb;
                }
              emit_road_surface_strip(&quad, source, pixel_control);

              quad.top_left = quad.top_right;
              quad.top_right.x += top_span;
              quad.bottom_left = quad.bottom_right;
              quad.bottom_right.x += bottom_span;
              strip_index++;
            }
        }

      if(strip_count >= 0)
        {
          quad.top_right.x += ROAD_TRANSITION_EDGE_INSET;
          quad.bottom_right.x += ROAD_TRANSITION_EDGE_INSET;

          if(surface_style < 0)
            {
              surface_style = select_road_surface_style(
                road_side, &quad, 0);
            }

          source = (CCB *)gRoadSurfaceCelVariants[surface_style][
            select_strip_variant(left_width, right_width, strip_index)];
          if(source == 0)
            {
              source = display_runtime_globals.fallback_ccb;
            }
          emit_road_surface_strip(&quad, source, pixel_control);
        }

      return (int)road_side;
    }

  strip_index = 0;
  if(strip_count > 0)
    {
      surface_style = select_road_surface_style(
        road_side, &quad, 0);
      if(style_cels != 0)
        {
          style_cel = (CCB *)style_cels[surface_style];
        }
      else
        {
          style_row = &variant_cels[
            surface_style * ROAD_SURFACE_CEL_VARIANTS];
        }

      while(strip_index < strip_count)
        {
          if(style_cels != 0)
            {
              source = style_cel;
            }
          else
            {
              source = (CCB *)style_row[select_strip_variant(
                left_width, right_width, strip_index)];
            }
          if(source == 0)
            {
              source = display_runtime_globals.fallback_ccb;
            }
          emit_road_surface_strip(&quad, source, pixel_control);

          quad.top_left = quad.top_right;
          quad.top_right.x += top_span;
          quad.bottom_left = quad.bottom_right;
          quad.bottom_right.x += bottom_span;
          strip_index++;
        }
    }

  if(strip_count >= 0)
    {
      quad.top_right.x += ROAD_TRANSITION_EDGE_INSET;
      quad.bottom_right.x += ROAD_TRANSITION_EDGE_INSET;

      if(surface_style < 0)
        {
          surface_style = select_road_surface_style(
            road_side, &quad, 0);
        }

      if(style_cels != 0)
        {
          source = (CCB *)style_cels[surface_style];
        }
      else
        {
          source = (CCB *)variant_cels[
            surface_style * ROAD_SURFACE_CEL_VARIANTS +
            select_strip_variant(left_width, right_width, strip_index)];
        }
      if(source == 0)
        {
          source = display_runtime_globals.fallback_ccb;
        }
      emit_road_surface_strip(&quad, source, pixel_control);
    }

  return (int)road_side;
}

#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "road_geometry.h"
#include "road_projected_cel.h"
#include "rw_semantic_data.h"

enum RoadProjectedCelConstant
{
  ROAD_PROJECTED_LITERAL_DIMENSION_SHIFT = 23,
  ROAD_PROJECTED_EXPONENT_MASK = 0x1F,
  ROAD_PROJECTED_DOUBLE_SCALE = 0x10000,
  ROAD_PROJECTED_QUADRUPLE_SCALE = 0x20000,
  ROAD_PROJECTED_REVERSE_HORIZONTAL = 1,
  ROAD_PROJECTED_INTERPOLATION_MASK = 0xFF,
  ROAD_PROJECTED_FIXED_FRACTION_BITS = 8,
  ROAD_PROJECTED_RECIPROCAL_INDEX_SHIFT = 1,
  ROAD_PROJECTED_SCREEN_CENTER_X = 0x960000,
  ROAD_PROJECTED_SCREEN_CENTER_Y = 0x5A0000,
  ROAD_PROJECTED_FALLBACK_SCALE = 0x270,
  ROAD_PROJECTED_BASE_SCALE_SHIFT = 6,
  ROAD_PROJECTED_POST_SCALE_SHIFT = 2,
  ROAD_PROJECTED_SCREEN_RIGHT = 0x12C0000,
  ROAD_PROJECTED_PHASE_CLASS = 3
};

#define ROAD_PROJECTED_PHASE_PIXC_STEP 0x01000001U

__inline
static
int
decode_projected_cel_dimension(int encoded_dimension)
{
  if(encoded_dimension >= 0)
    {
      return (int)(((unsigned int)encoded_dimension <<
                    ROAD_PROJECTED_LITERAL_DIMENSION_SHIFT) >>
                   ROAD_PROJECTED_LITERAL_DIMENSION_SHIFT);
    }
  return 1 << (encoded_dimension & ROAD_PROJECTED_EXPONENT_MASK);
}


void
render_road_projected_cel(RoadProjectedRenderContext *render_context,
                          RoadProjectedCelDescriptor *descriptor,
                          int                         base_scale,
                          int                         depth,
                          int                         vertical_position,
                          int                         interpolation,
                          int                         mode)
{
  RoadProjectedRenderSample *target;
  RoadProjectedCelExtents extents;
  RoadCelBounds bounds;
  CCB *source;
  CCB *packet;
  SignedDivisionResult division;
  unsigned int flags;
  int frame;
  int render_mode;
  int scale;
  int dimension;
  int dimension_again;
  int step;
  int projected_x;
  int projected_y;

  source = descriptor->source_ccb;
  if(source == 0)
    {
      return;
    }

  flags = descriptor->flags;
  if((flags & ROAD_PROJECTED_DOUBLE_SCALE) != 0)
    {
      base_scale <<= 1;
    }
  if((flags & ROAD_PROJECTED_QUADRUPLE_SCALE) != 0)
    {
      base_scale <<= ROAD_PROJECTED_POST_SCALE_SHIFT;
    }

  extents.horizontal = descriptor->horizontal_extent;
  extents.vertical = descriptor->vertical_extent;
  if((flags & ROAD_PROJECTED_REVERSE_HORIZONTAL) != 0)
    {
      extents.horizontal =
        decode_projected_cel_dimension(source->ccb_Width) -
        extents.horizontal;
    }

  frame = interpolation & ROAD_PROJECTED_INTERPOLATION_MASK;
  render_mode = mode & ROAD_PROJECTED_INTERPOLATION_MASK;
  target = render_context->interpolation_target;
  projected_x = depth +
                ((target->projected_x - render_context->projected_x) * frame >>
                 ROAD_PROJECTED_FIXED_FRACTION_BITS) +
                render_context->projected_x;
  projected_y = vertical_position +
                ((target->projected_y - render_context->projected_y) * frame >>
                 ROAD_PROJECTED_FIXED_FRACTION_BITS) +
                render_context->projected_y;

  scale = road_renderer_state.reciprocal_table[
    (frame + render_context->reciprocal_index_bias) >>
    ROAD_PROJECTED_RECIPROCAL_INDEX_SHIFT];
  projected_x = scale *
                (projected_x - road_renderer_state.motion.projection_origin_x) +
                ROAD_PROJECTED_SCREEN_CENTER_X;
  projected_y = scale *
                (road_renderer_state.motion.projection_origin_y - projected_y) +
                ROAD_PROJECTED_SCREEN_CENTER_Y;

  if(base_scale == 0)
    {
      dimension =
        (track_simulation_globals.speed_scale *
         ROAD_PROJECTED_FALLBACK_SCALE) >>
        ROAD_PROJECTED_FIXED_FRACTION_BITS;
      division = signed_divide_with_remainder(
        (unsigned int)decode_projected_cel_dimension(source->ccb_Width),
        (unsigned int)(scale * dimension));
      step = division.quotient;
    }
  else
    {
      step = (scale *
              (base_scale >> ROAD_PROJECTED_BASE_SCALE_SHIFT)) >>
             ROAD_PROJECTED_POST_SCALE_SHIFT;
    }

  dimension = decode_projected_cel_dimension(source->ccb_Height);
  dimension_again = decode_projected_cel_dimension(source->ccb_Height);
  if(dimension_again == extents.vertical)
    {
      extents.vertical = step * dimension;
    }
  else if(extents.vertical != 0)
    {
      extents.vertical = step * extents.vertical;
    }

  bounds.top = projected_y - extents.vertical;
  if(bounds.top < render_context->clip->top ||
     render_mode == 0)
    {
      bounds.bottom = bounds.top + step * dimension;

      dimension = decode_projected_cel_dimension(source->ccb_Width);
      dimension_again = decode_projected_cel_dimension(source->ccb_Width);
      if(dimension_again == extents.horizontal)
        {
          extents.horizontal = step * dimension;
        }
      else if(extents.horizontal != 0)
        {
          extents.horizontal = step * extents.horizontal;
        }

      projected_x -= extents.horizontal;
      bounds.right = projected_x + step * dimension;
      if(bounds.right > 0 &&
         projected_x < ROAD_PROJECTED_SCREEN_RIGHT)
        {
          if((flags & ROAD_PROJECTED_REVERSE_HORIZONTAL) == 0)
            {
              bounds.left = projected_x;
            }
          else
            {
              bounds.left = bounds.right;
              bounds.right = projected_x;
            }

          packet = append_unmapped_cel(source);
          if(render_context->render_class->kind ==
             ROAD_PROJECTED_PHASE_CLASS &&
             target->render_class->kind ==
             ROAD_PROJECTED_PHASE_CLASS)
            {
              packet->ccb_PIXC += ROAD_PROJECTED_PHASE_PIXC_STEP;
            }

          if(descriptor->mapping_parameter != 0)
            {
              map_rotated_road_bounds_to_cel(
                packet, &bounds, &extents,
                descriptor->mapping_parameter);
            }
          else
            {
              map_road_bounds_to_cel(packet, &bounds);
            }
        }
    }
}


#undef ROAD_PROJECTED_PHASE_PIXC_STEP

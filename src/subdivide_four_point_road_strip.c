/* Reconstructed from the original function at 0x00033760. */



#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

CelData *
select_cel_source_row(CCB *cel,
                      int  source_row);

static
int
shift_value(int value)
{
  if(value < 0)
    {
      return 1 << (value & ROAD_SHIFT_COUNT_MASK);
    }
  return value & ROAD_CEL_SCALE_MASK;
}


static
int
div4_towards_zero(int value)
{
  if(value < 0)
    {
      value += ROAD_SUBDIVISION_QUARTER_ROUNDING_BIAS;
    }
  return value >> ROAD_SUBDIVISION_QUARTER_SHIFT;
}


static
int
scaled_depth(int scale,
                   int delta)
{
  int value;

  value = scale * delta;
  if(delta < 0)
    {
      if(value < 0)
        {
          value += ROAD_PROJECTION_SCREEN_X_CENTER;
        }
      else
        {
          value = ROAD_PROJECTION_PRODUCT_MINIMUM;
        }
    }
  else
    {
      value += ROAD_PROJECTION_SCREEN_X_CENTER;
      if(value < 0)
        {
          value = ROAD_PROJECTION_PRODUCT_LIMIT;
        }
    }
  return value;
}


void
subdivide_four_point_road_strip(RoadStripDrawContext *context)
{
  CCB *cel;
  RoadQuad *source;
  CCB *packet;
  RoadQuad projected;
  int x_step;
  int y_step;
  int z0_step;
  int z1_step;
  int x_base;
  int draw_index;
  int scale_value;
  int scale_step;
  unsigned int shade_bits;
  int count;
  int sample;
  int value;
  int moving_positive;
  int bottom_left_x;
  int bottom_left_y;
  int bottom_right_x;
  int bottom_right_y;

  cel = context->cel;
  if(cel == 0)
    {
      return;
    }

  draw_index = 0;
  scale_value = shift_value(cel->ccb_Height);
  scale_step = div4_towards_zero(scale_value);

  x_base = context->projection_depth - ROAD_SUBDIVISION_DEPTH_STEP;
  x_step = (context->world_quad.bottom_left.x -
            context->world_quad.top_left.x) >>
           ROAD_SUBDIVISION_QUARTER_SHIFT;
  y_step = (context->world_quad.bottom_left.y -
            context->world_quad.top_left.y) >>
           ROAD_SUBDIVISION_QUARTER_SHIFT;
  z0_step = (context->world_quad.bottom_right.x -
             context->world_quad.top_right.x) >>
            ROAD_SUBDIVISION_QUARTER_SHIFT;
  z1_step = (context->world_quad.bottom_right.y -
             context->world_quad.top_right.y) >>
            ROAD_SUBDIVISION_QUARTER_SHIFT;
  moving_positive = context->world_quad.top_right.x >
                    context->world_quad.top_left.x;

  cel->ccb_Height -= ROAD_SUBDIVISION_CEL_HEIGHT_ADJUST;

  source = context->screen_quad;
  projected.top_left = source->top_left;
  projected.top_right = source->top_right;

  count = ROAD_SUBDIVISION_INTERMEDIATE_COUNT;
  shade_bits = ROAD_PRE0_SHADE_BITS(scale_step);
  do
    {
      if(x_base < ROAD_SUBDIVISION_MIN_DEPTH)
        {
          break;
        }
      if(projected.top_left.y >= context->horizon_y &&
         projected.top_right.y >= context->horizon_y)
        {
          break;
        }
      if(moving_positive)
        {
          if(projected.top_left.x >= ROAD_SCREEN_RIGHT)
            {
              break;
            }
        }
      else if(projected.top_left.x <= 0)
        {
          break;
        }

      if(count == 0)
        {
          projected.bottom_right = source->bottom_right;
          projected.bottom_left = source->bottom_left;
        }
      else
        {
          sample = road_renderer_state.reciprocal_table[x_base >> 1];
          value = context->world_quad.top_left.x + x_step;
          context->world_quad.top_left.x = value;
          bottom_left_x = scaled_depth(
            sample,
            value - road_renderer_state.motion.projection_origin_x);

          value = context->world_quad.top_left.y + y_step;
          context->world_quad.top_left.y = value;
          bottom_left_y =
            sample *
            (road_renderer_state.motion.projection_origin_y - value) +
            ROAD_PROJECTION_SCREEN_Y_ORIGIN;

          value = context->world_quad.top_right.x + z0_step;
          context->world_quad.top_right.x = value;
          bottom_right_x = scaled_depth(
            sample,
            value - road_renderer_state.motion.projection_origin_x);

          value = context->world_quad.top_right.y + z1_step;
          context->world_quad.top_right.y = value;
          bottom_right_y =
            sample *
            (road_renderer_state.motion.projection_origin_y - value) +
            ROAD_PROJECTION_SCREEN_Y_ORIGIN;

          /* The edge bias is applied to the locals: adjusting the stored
             fields instead made the compiler reload each field, alter it and
             store it back through the address-taken quad. */
          if(context->adjust_edges != 0)
            {
              bottom_left_y -= ROAD_FIXED_THREE_QUARTERS;
              bottom_right_y += ROAD_FIXED_THREE_QUARTERS;
              if(context->side == ROAD_SIDE_LEFT)
                {
                  bottom_left_x -= ROAD_FIXED_HALF;
                  bottom_right_x -= ROAD_FIXED_HALF;
                }
              else
                {
                  bottom_left_x += ROAD_FIXED_HALF;
                  bottom_right_x += ROAD_FIXED_HALF;
                }
            }

          projected.bottom_left.x = bottom_left_x;
          projected.bottom_left.y = bottom_left_y;
          projected.bottom_right.x = bottom_right_x;
          projected.bottom_right.y = bottom_right_y;
        }

      packet = append_mapped_quad_cel(
        cel, (const DisplayQuad *)(const void *)&projected);
      packet->ccb_SourcePtr = select_cel_source_row(packet, draw_index);
      packet->ccb_PRE0 =
        (packet->ccb_PRE0 & ~PRE0_VCNT_MASK) | shade_bits;

      projected.top_left = projected.bottom_left;
      projected.top_right = projected.bottom_right;
      x_base -= ROAD_SUBDIVISION_DEPTH_STEP;
      draw_index += scale_step;
      count--;
    } while(count >= 0);

  cel->ccb_Height += ROAD_SUBDIVISION_CEL_HEIGHT_ADJUST;
}

#include "cans_animation_binding.h"

enum CansRenderOrientationConstant
{
  CANS_RENDER_PHASE_MASK = 0xFF,
  CANS_RENDER_FIXED_SHIFT = 8,
  CANS_RENDER_FIXED_ROUNDING_BIAS = 0xFF,
  CANS_RENDER_DOUBLE_WIDTH_CLASS = 8,
  CANS_RENDER_ANGLE_HALF_TURN = 0x800000,
  CANS_RENDER_ANGLE_FULL_TURN = 0x1000000
};

int
fixed_vector_angle_24(int horizontal,
                      int vertical);

void
calculate_cans_render_orientation(CansAnimationRenderObject        *object,
                                  CansRenderOrientation            *orientation,
                                  const RoadProjectedRenderContext *render_context)
{
  int horizontal;
  int vertical;
  int object_angle;
  int camera_angle;

  horizontal = (object->render_phase & CANS_RENDER_PHASE_MASK) +
               render_context->reciprocal_index_bias;
  if(object->render_class == CANS_RENDER_DOUBLE_WIDTH_CLASS)
    {
      horizontal <<= 1;
    }

  vertical = object->depth_fraction;
  if(vertical < 0)
    {
      vertical += CANS_RENDER_FIXED_ROUNDING_BIAS;
    }
  vertical = render_context->projected_x +
             (vertical >> CANS_RENDER_FIXED_SHIFT) -
             road_renderer_state.motion.projection_origin_x;
  object_angle = fixed_vector_angle_24(horizontal, vertical);
  if(object_angle > CANS_RENDER_ANGLE_HALF_TURN)
    {
      object_angle -= CANS_RENDER_ANGLE_FULL_TURN;
    }

  camera_angle = render_context->view_angle;
  if(camera_angle > CANS_RENDER_ANGLE_HALF_TURN)
    {
      camera_angle -= CANS_RENDER_ANGLE_FULL_TURN;
    }
  orientation->horizontal = 0;
  orientation->vertical = camera_angle + object->tilt_angle - object_angle;
  orientation->bank = object->bank_angle;
  object->view_angle = object_angle;
}

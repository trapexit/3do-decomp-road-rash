
#include "cans_animation_binding.h"
#include "rider_animation_runtime.h"
#include "road_projected_cel.h"
#include "rw_semantic_data.h"

enum CarAnimationRenderValue
{
  CAR_ANIMATION_RENDER_FIXED_SHIFT = 8,
  CAR_ANIMATION_RENDER_ROUNDING_BIAS = 0xFF,
  CAR_ANIMATION_RENDER_BASE_SCALE = 0x500,
  CAR_ANIMATION_RENDER_MODE = 1
};

int
initialize_car_animation(RiderAnimationRuntime *animation,
                         int                    animation_index)
{
  const CarAnimationResourcePair *resources;
  int index;

  index = (signed char)animation_index;
  resources = &car_animation_resources[index];
  animation->cans_resource = resources->cans_resource;
  animation->animation_resource = resources->animation_resource;
  animation->state_table = car_animation_bindings_by_car[index];

  set_primary_rider_animation(animation, -1);
  set_secondary_rider_animation(animation, -1);
  animation->current_frame = 0;
  return advance_car_animation(animation);
}


int
car_animation_render(CansAnimationRenderObject  *object,
                     RoadProjectedRenderContext *render_context)
{
  int depth;

  prepare_cans_animation_frame_render(object, render_context);
  if(object->projected_cel.source_ccb == 0)
    {
      return 0;
    }

  depth = object->depth_fraction;
  if(depth < 0)
    {
      depth += CAR_ANIMATION_RENDER_ROUNDING_BIAS;
    }

  render_road_projected_cel(
    render_context, &object->projected_cel,
    CAR_ANIMATION_RENDER_BASE_SCALE,
    depth >> CAR_ANIMATION_RENDER_FIXED_SHIFT,
    object->vertical_position_fraction >>
      CAR_ANIMATION_RENDER_FIXED_SHIFT,
    object->render_phase,
    CAR_ANIMATION_RENDER_MODE);
  return 0;
}

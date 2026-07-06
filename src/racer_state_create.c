#include "stdlib.h"

#include "bss_early_data.h"
#include "cans_family_validation.h"
#include "family_resource_runtime.h"
#include "racer_auxiliary_cache.h"
#include "racer_render_runtime.h"

int
absolute_int_difference(int first,
                        int second);

enum RacerRenderCreationConstant
{
  RACER_RENDER_FIXED_FRACTION_BITS = 8,
  RACER_RENDER_FIXED_ROUNDING_BIAS = 0xFF,
  RACER_RENDER_PROJECT_SCALE = 0x200,
  RACER_RENDER_TILT_DEPTH_THRESHOLD = 0xE0000,
  RACER_RENDER_DEPTH_LIMIT = 0x50000,
  RACER_AUXILIARY_FRAME_LIMIT = 3,
  RACER_AUXILIARY_RESOURCE_ENTRY_COUNT = 2,
  RACER_AUXILIARY_RESOURCE_ENTRY_STRIDE = 44,
  RACER_AUXILIARY_RESOURCE_SELECTOR_OFFSET = 0x40,
  RACER_INTERPOLATION_FRACTION_MASK = 0xFF,
  RACER_INTERPOLATION_FRACTION_LIMIT = 0x80,
  RACER_PROJECT_PRIMARY_MODE = 0,
  RACER_PROJECT_SECONDARY_MODE = 1
};

static
int
divide_render_fixed_towards_zero(int value)
{
  if(value < 0)
    {
      value += RACER_RENDER_FIXED_ROUNDING_BIAS;
    }
  return value >> RACER_RENDER_FIXED_FRACTION_BITS;
}


static
void
select_racer_render_resource(RacerRenderObject *object,
                             int                compare_object)
{
  RacerRenderObject *challenge;
  RacerRenderObject *challenge_state;

  challenge = (RacerRenderObject *)race_rider_state.challenge_object;
  if(challenge != 0)
    {
      challenge_state = challenge->paired_state;
      if((compare_object != 0 && object == challenge_state) ||
         (compare_object == 0 &&
          object->paired_state == challenge_state))
        {
          object->render_resources.animation_resource =
            secondary_animation_resource_pairs[1].animation_resource;
          return;
        }
    }

  object->render_resources.animation_resource =
    secondary_animation_resource_pairs[0].animation_resource;
}


static
unsigned char
racer_auxiliary_resource_selector(const RacerRenderObject *object,
                                  int                      entry_index)
{
  return object->resource_entry_table[
    RACER_AUXILIARY_RESOURCE_SELECTOR_OFFSET +
    entry_index * RACER_AUXILIARY_RESOURCE_ENTRY_STRIDE];
}


void
racer_object_create(RacerRenderObject  *object,
                    RacerRenderContext *render_context)
{
  void *saved_animation_resource;
  int depth;
  int vertical_position;

  saved_animation_resource = object->render_resources.animation_resource;
  select_racer_render_resource(object, 0);

  prepare_cans_animation_frame_render(
    (CansAnimationRenderObject *)object,
    (RoadProjectedRenderContext *)render_context);
  object->projected_cel.mapping_parameter = 0;
  if(object->projected_cel.source_ccb == 0)
    {
      return;
    }

  depth = divide_render_fixed_towards_zero(
    object->render_depth_fraction);
  render_road_projected_cel(
    (RoadProjectedRenderContext *)render_context,
    &object->projected_cel,
    RACER_RENDER_PROJECT_SCALE, depth, object->render_y,
    object->render_interpolation, RACER_PROJECT_PRIMARY_MODE);

  object->render_resources.animation_resource = saved_animation_resource;
  prepare_cans_animation_frame_render(
    (CansAnimationRenderObject *)object,
    (RoadProjectedRenderContext *)render_context);
  object->projected_cel.mapping_parameter = 0;
  if(object->projected_cel.source_ccb == 0)
    {
      return;
    }

  if(object->projected_cel.palette != 0)
    {
      object->projected_cel.source_ccb->ccb_PLUTPtr =
        object->projected_cel.palette;
    }

  vertical_position = object->render_y +
                      (object->render_y_fraction >> RACER_RENDER_FIXED_FRACTION_BITS);
  depth = divide_render_fixed_towards_zero(
    object->render_depth_fraction);
  render_road_projected_cel(
    (RoadProjectedRenderContext *)render_context,
    &object->projected_cel,
    RACER_RENDER_PROJECT_SCALE, depth, vertical_position,
    object->render_interpolation, RACER_PROJECT_SECONDARY_MODE);
}


void
racer_state_create(RacerRenderObject  *object,
                   RacerRenderContext *render_context)
{
  void *saved_animation_resource;
  RacerRenderObject *owner;
  CansAnimationData *animation;
  RacerAuxiliaryProjectedFrame auxiliary;
  int depth;
  int vertical_position;
  int absolute_tilt;
  int auxiliary_frame_count;
  int resource_selector;
  int entry_index;
  int interpolation;

  saved_animation_resource = object->render_resources.animation_resource;
  select_racer_render_resource(object, 1);

  prepare_cans_animation_frame_render(
    (CansAnimationRenderObject *)object,
    (RoadProjectedRenderContext *)render_context);
  object->projected_cel.mapping_parameter = 0;
  if(object->projected_cel.source_ccb != 0)
    {
      depth = divide_render_fixed_towards_zero(
        object->render_depth_fraction);
      render_road_projected_cel(
        (RoadProjectedRenderContext *)render_context,
        &object->projected_cel,
        RACER_RENDER_PROJECT_SCALE, depth, object->render_y,
        object->render_interpolation, RACER_PROJECT_PRIMARY_MODE);
    }

  object->render_resources.animation_resource = saved_animation_resource;
  prepare_cans_animation_frame_render(
    (CansAnimationRenderObject *)object,
    (RoadProjectedRenderContext *)render_context);
  if(object->projected_cel.source_ccb == 0)
    {
      return;
    }

  if(object->projected_cel.palette != 0)
    {
      object->projected_cel.source_ccb->ccb_PLUTPtr =
        object->projected_cel.palette;
    }

  absolute_tilt = absolute_int_difference(object->render_tilt, 0);
  if(absolute_tilt > RACER_RENDER_TILT_DEPTH_THRESHOLD)
    {
      if(object->projected_cel.mapping_parameter >
         RACER_RENDER_DEPTH_LIMIT)
        {
          object->projected_cel.mapping_parameter =
            RACER_RENDER_DEPTH_LIMIT;
        }
      else if(object->projected_cel.mapping_parameter <
              -RACER_RENDER_DEPTH_LIMIT)
        {
          object->projected_cel.mapping_parameter =
            -RACER_RENDER_DEPTH_LIMIT;
        }
    }

  vertical_position = object->render_y +
                      (object->render_y_fraction >> RACER_RENDER_FIXED_FRACTION_BITS);
  depth = divide_render_fixed_towards_zero(
    object->render_depth_fraction);
  render_road_projected_cel(
    (RoadProjectedRenderContext *)render_context,
    &object->projected_cel,
    RACER_RENDER_PROJECT_SCALE, depth, vertical_position,
    object->render_interpolation, RACER_PROJECT_SECONDARY_MODE);

  owner = object->owner;
  if(owner->render_binding != 0)
    {
      return;
    }

  auxiliary_frame_count = owner->auxiliary_frame_count;
  if(auxiliary_frame_count <= 0 ||
     auxiliary_frame_count > RACER_AUXILIARY_FRAME_LIMIT)
    {
      return;
    }

  resource_selector = owner->auxiliary_resource_id;
  for(entry_index = 0;
      entry_index < RACER_AUXILIARY_RESOURCE_ENTRY_COUNT;
      entry_index++)
    {
      if(racer_auxiliary_resource_selector(object, entry_index) ==
         resource_selector)
        {
          break;
        }
    }
  if(entry_index == RACER_AUXILIARY_RESOURCE_ENTRY_COUNT)
    {
      return;
    }

  animation = resolve_racer_auxiliary_animation(
    render_context->texture_cache, entry_index,
    (unsigned int)resource_selector);
  if(animation == 0)
    {
      return;
    }
  if(family_cans_animation_has_cached_valid_root(
       (unsigned int)resource_selector, animation) == 0)
    {
      invalidate_racer_auxiliary_animation(
        render_context->texture_cache, entry_index);
      return;
    }

  CANS_GetAnimFrame(animation,
                   (auxiliary_frame_count - 1),
                   &auxiliary.animation_frame);
  if(auxiliary.animation_frame.ccb == 0)
    {
      invalidate_racer_auxiliary_animation(
        render_context->texture_cache, entry_index);
      return;
    }

  auxiliary.projected_cel.source_ccb = auxiliary.animation_frame.ccb;
  auxiliary.projected_cel.horizontal_extent =
    auxiliary.animation_frame.center_x;
  auxiliary.projected_cel.vertical_extent =
    auxiliary.animation_frame.center_y;
  auxiliary.projected_cel.flags = 0;
  auxiliary.projected_cel.mapping_parameter = 0;

  interpolation = object->render_interpolation;
  if((interpolation & RACER_INTERPOLATION_FRACTION_MASK) >
     RACER_INTERPOLATION_FRACTION_LIMIT)
    {
      interpolation =
        (interpolation & ~RACER_INTERPOLATION_FRACTION_MASK) |
        RACER_INTERPOLATION_FRACTION_LIMIT;
    }

  vertical_position = object->render_y +
                      (object->render_y_fraction >> RACER_RENDER_FIXED_FRACTION_BITS);
  depth = divide_render_fixed_towards_zero(
    object->render_depth_fraction);
  render_road_projected_cel(
    (RoadProjectedRenderContext *)render_context,
    &auxiliary.projected_cel,
    RACER_RENDER_PROJECT_SCALE, depth, vertical_position,
    interpolation, RACER_PROJECT_SECONDARY_MODE);
}

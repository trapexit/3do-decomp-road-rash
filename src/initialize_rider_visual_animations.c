#include "family_resource_binding.h"
#include "family_resource_runtime.h"
#include "rider_animation_runtime.h"
#include "rider_visual_resource_binding.h"
#include "rider_visual_runtime.h"
#include "rw_semantic_data.h"

#define INITIALIZE_RIDER_ANIMATION_STATE(state, frame_specs, bindings)       \
        if(resolve_cans_animation_bindings(                                    \
             &resources, (frame_specs), (bindings),                          \
             sizeof(frame_specs) / sizeof((frame_specs)[0])) != 0) {         \
            rider->animation_states[(state)] = (bindings);                      \
          } else {                                                               \
            rider->animation_states[(state)] = 0;                               \
          }

void
initialize_rider_visual_animations(RiderVisualObject *rider)
{
  RiderVisualPoolSlot *slot;
  RiderAnimationRuntime *animation;
  AnimationResourcePair resources;
  FamilyResourceBindingIdentity animation_identity;
  FamilyResourceBindingIdentity cans_identity;

  animation = &rider->animation;
  if(rider->animation_ready != 0)
    {
      return;
    }

  resources.animation_resource =
    (void *)lookup_family_resource_child_for_render_with_identity(
      rider->family_resource_inventory, RIDER_FAMILY_ITEM_TYPE,
      RIDER_FAMILY_ANIMATION_SLOT, &animation_identity);
  resources.cans_resource =
    (void *)lookup_family_resource_child_for_render_with_identity(
      rider->family_resource_inventory, RIDER_FAMILY_ITEM_TYPE,
      RIDER_FAMILY_CANS_SLOT, &cans_identity);
  if(resources.cans_resource == 0 || resources.animation_resource == 0 ||
     animation_identity.selector != cans_identity.selector ||
     animation_identity.generation != cans_identity.generation)
    {
      return;
    }

  slot = (RiderVisualPoolSlot *)(void *)rider;
  animation->cans_resource = resources.cans_resource;
  animation->animation_resource = resources.animation_resource;
  animation->state_table = rider->animation_states;
  set_primary_rider_animation(animation, RIDER_ANIMATION_STANDING);
  set_secondary_rider_animation(animation, RIDER_ANIMATION_NONE);
  animation->current_frame = 0;
  advance_car_animation(animation);

  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_MOTION, rider_motion_frames, slot->motion_bindings)
  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_STANDING, rider_standing_frames,
    slot->standing_bindings)
  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_SHAKE, rider_shake_frames, slot->shake_bindings)
  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_FALL, rider_fall_frames, slot->fall_bindings)
  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_ATTACK, rider_attack_frames, slot->attack_bindings)
  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_FAST, rider_fast_frames, slot->fast_bindings)
  INITIALIZE_RIDER_ANIMATION_STATE(
    RIDER_ANIMATION_FLAG, rider_flag_frames, slot->flag_bindings)

  animation->cans_resource = resources.cans_resource;
  animation->animation_resource = resources.animation_resource;
  animation->state_table = rider->animation_states;
  set_primary_rider_animation(animation, RIDER_ANIMATION_STANDING);
  set_secondary_rider_animation(animation, RIDER_ANIMATION_NONE);
  animation->current_frame = 0;
  advance_car_animation(animation);

  rider->animation_ready = 1;
  record_rider_visual_resource_binding(rider, &animation_identity);
}


#undef INITIALIZE_RIDER_ANIMATION_STATE

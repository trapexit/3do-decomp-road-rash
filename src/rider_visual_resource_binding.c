#include "rider_visual_resource_binding.h"

typedef struct RiderVisualResourceBindingState
{
  RiderVisualObject *owner;
  unsigned int generation;
  unsigned char selector;
} RiderVisualResourceBindingState;

/* RiderVisualObject has an exact oracle layout.  Keep reusable-family
   ownership outside the 0x460-byte pool slot. */
static RiderVisualResourceBindingState
  rider_visual_resource_bindings[RIDER_VISUAL_POOL_CAPACITY];

__inline
static
RiderVisualResourceBindingState *
rider_visual_resource_binding(RiderVisualObject *rider,
                              int                create)
{
  RiderVisualResourceBindingState *empty;
  int index;

  empty = 0;
  for(index = 0; index < RIDER_VISUAL_POOL_CAPACITY; index++)
    {
      if(rider_visual_resource_bindings[index].owner == rider)
        {
          return &rider_visual_resource_bindings[index];
        }
      if(empty == 0 &&
         rider_visual_resource_bindings[index].owner == 0)
        {
          empty = &rider_visual_resource_bindings[index];
        }
    }

  if(create != 0 && empty != 0)
    {
      empty->owner = rider;
      empty->selector = 0;
      empty->generation = 0;
      return empty;
    }
  return 0;
}


__inline
static
void
invalidate_rider_visual_resource_binding(RiderVisualObject               *rider,
                                         RiderVisualResourceBindingState *binding)
{
  int state;

  rider->animation_ready = 0;
  rider->animation.cans_resource = 0;
  rider->animation.animation_resource = 0;
  rider->animation.state_table = rider->animation_states;
  rider->animation.current_frame = 0;
  rider->render_resources.cans_resource = 0;
  rider->render_resources.animation_resource = 0;
  rider->render_binding = 0;
  rider->render_ccb = 0;
  for(state = 0; state < RIDER_ANIMATION_STATE_COUNT; state++)
    {
      rider->animation_states[state] = 0;
    }

  if(binding != 0)
    {
      binding->selector = 0;
      binding->generation = 0;
    }
}


void
reset_all_rider_visual_resource_bindings(void)
{
  int index;

  for(index = 0; index < RIDER_VISUAL_POOL_CAPACITY; index++)
    {
      rider_visual_resource_bindings[index].owner = 0;
      rider_visual_resource_bindings[index].selector = 0;
      rider_visual_resource_bindings[index].generation = 0;
    }
}


void
reset_rider_visual_resource_binding(RiderVisualObject *rider)
{
  RiderVisualResourceBindingState *binding;

  if(rider == 0)
    {
      return;
    }
  binding = rider_visual_resource_binding(rider, 1);
  if(binding != 0)
    {
      binding->selector = 0;
      binding->generation = 0;
    }
}


void
record_rider_visual_resource_binding(RiderVisualObject                   *rider,
                                     const FamilyResourceBindingIdentity *identity)
{
  RiderVisualResourceBindingState *binding;

  if(rider == 0 || identity == 0 || identity->generation == 0)
    {
      return;
    }
  binding = rider_visual_resource_binding(rider, 1);
  if(binding != 0)
    {
      binding->selector = (unsigned char)identity->selector;
      binding->generation = identity->generation;
    }
}


int
refresh_rider_visual_resource_binding(RiderVisualObject *rider)
{
  RiderVisualResourceBindingState *binding;

  if(rider == 0 || rider->animation_ready == 0)
    {
      return 0;
    }

  binding = rider_visual_resource_binding(rider, 0);
  if(binding != 0 &&
     family_resource_binding_values_are_current(
       binding->selector, binding->generation) != 0)
    {
      return 1;
    }

  invalidate_rider_visual_resource_binding(rider, binding);
  return 0;
}

#include "rider_animation_runtime.h"

enum
{
  RIDER_ANIMATION_INITIAL_STATE = 2
};

static const signed char *const rider_animation_variant_map_reference =
  animation_resource_pair_by_variant;
static CelAnimationBinding ***const rider_animation_state_tables_reference =
  rider_animation_state_tables;

int
initialize_rider_animation(RiderAnimationRuntime *animation,
                           RacerRole              role)
{
  const AnimationResourcePair *resources;
  int resource_pair_index;

  resource_pair_index =
    rider_animation_variant_map_reference[role];
  resources = &primary_animation_resource_pairs[resource_pair_index];
  animation->cans_resource = resources->cans_resource;
  animation->animation_resource = resources->animation_resource;
  animation->state_table =
    rider_animation_state_tables_reference[resource_pair_index];

  set_primary_rider_animation(animation, RIDER_ANIMATION_INITIAL_STATE);
  set_secondary_rider_animation(animation, RIDER_ANIMATION_INITIAL_STATE);
  animation->current_frame = 0;
  return advance_car_animation(animation);
}

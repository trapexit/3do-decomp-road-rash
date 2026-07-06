#include "rw_semantic_data.h"

enum RiderAnimationResourcePairConstant
{
  RIDER_ANIMATION_RESOURCE_PAIR_COUNT = 2
};

void
clear_rider_animation_resource_pairs(void)
{
  int pair_index;

  for(pair_index = 0;
      pair_index < RIDER_ANIMATION_RESOURCE_PAIR_COUNT;
      pair_index++)
    {
      primary_animation_resource_pairs[pair_index].cans_resource = 0;
      primary_animation_resource_pairs[pair_index].animation_resource = 0;
      secondary_animation_resource_pairs[pair_index].cans_resource = 0;
      secondary_animation_resource_pairs[pair_index].animation_resource = 0;
    }
}

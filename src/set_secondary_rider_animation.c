#include "rider_animation_runtime.h"

int
set_secondary_rider_animation(RiderAnimationRuntime *animation,
                              int                    state)
{
  CelAnimationBinding *binding;
  int selector;

  selector = (signed char)state;
  if(selector != -1)
    {
      binding = animation->state_table[selector];
      if(binding == 0 || binding->frame_flags == 0)
        {
          return 0;
        }
    }
  animation->secondary_state = (unsigned char)selector;
  return 1;
}

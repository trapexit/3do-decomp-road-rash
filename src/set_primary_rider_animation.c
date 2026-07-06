
#include "rider_animation_runtime.h"
#include "rw_semantic_data.h"

enum
{
  RIDER_ANIMATION_RESET_DELAY = 0xE10
};

int
set_primary_rider_animation(RiderAnimationRuntime *animation,
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

  animation->primary_state = (unsigned char)selector;
  animation->primary_frame = 0;
  animation->cycle_count = 0;

  if(selector != -1)
    {
      animation->next_frame_tick = -1;
    }
  else
    {
      animation->next_frame_tick =
        frame_tick + RIDER_ANIMATION_RESET_DELAY;
    }

  return 1;
}

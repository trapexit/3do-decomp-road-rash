#include "rider_animation_runtime.h"
#include "rw_semantic_data.h"

void
rider_update_mode_9(char *rider)
{
  (void)rider;
}


int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state)
{
  int index;

  index = (signed char)state;
  if(set_primary_rider_animation(animation, index) != 0)
    {
      return 1;
    }

  return set_primary_rider_animation(
    animation, rider_animation_fallback_states[index]);
}

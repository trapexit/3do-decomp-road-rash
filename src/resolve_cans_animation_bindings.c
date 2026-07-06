#include "cans_animation_binding.h"

int
resolve_cans_animation_bindings(const AnimationResourcePair *resources,
                                const AnimationBindingSpec  *specs,
                                CelAnimationBinding         *bindings,
                                int                          count)
{
  int index;
  int remaining;

  if(count == 0)
    {
      return 1;
    }

  index = 0;
  remaining = count - 1;
  do
    {
      if(!resolve_cans_animation_binding(resources, &specs[index],
                                         &bindings[index]))
        {
          return 0;
        }
      index++;
      if(remaining == 0)
        {
          break;
        }
      remaining--;
    } while(1);

  return 1;
}

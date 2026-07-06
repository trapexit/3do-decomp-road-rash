#include "cans_animation_binding.h"

int
get_cans_animation_entry_flags(const AnimationResourcePair *resources,
                               unsigned int                 entry_index,
                               int                          channel)
{
  return get_cans_animation_flag_table(resources, entry_index)[channel + 1];
}

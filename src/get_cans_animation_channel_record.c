#include "cans_animation_binding.h"

const
CansAnimationChannelRecord *
get_cans_animation_channel_record(const AnimationResourcePair *resources,
                                  int                          entry_index,
                                  int                          channel)
{
  const CansAnimationGroup *group;

  if(channel < 0)
    {
      return 0;
    }
  group = find_cans_animation_group(resources, entry_index);
  if(group == 0 || channel >= group->variant_count)
    {
      return 0;
    }
  return (const CansAnimationChannelRecord *)(group + 1) + channel;
}

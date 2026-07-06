/* Reconstructed from the original function at 0x00005858. */



#include "rider_contact_runtime.h"
#include "rw_semantic_data.h"

boolean
is_rider_attack_animation(const RacerEntity *rider)
{
  const RacerEntity *state;
  int index;
  int value;

  state = rider->paired_state;
  index = (signed char)state->attack_style;
  value = (signed char)state->animation.primary_state;

  if(opponent_resource_group_a[index] == value)
    {
      return true;
    }
  if(opponent_resource_group_b[index] == value)
    {
      return true;
    }
  if(opponent_resource_group_a[index + RIDER_ATTACK_STYLE_COUNT] == value)
    {
      return true;
    }
  if(opponent_resource_group_b[index + RIDER_ATTACK_STYLE_COUNT] == value)
    {
      return true;
    }
  return false;
}

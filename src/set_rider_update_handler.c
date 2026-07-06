#include "rider_contact_runtime.h"

void
set_rider_update_handler(RacerEntity *rider,
                         int          handler_index,
                         RacerEntity *target)
{
  RiderInteractionRuntime *interaction;
  signed char elapsed_index;

  interaction = rider->paired_state->interaction_data;
  if(interaction->handler_index == handler_index)
    {
      return;
    }

  interaction->handler_index = handler_index;
  interaction->handler_phase = 0;

  elapsed_index = (signed char)interaction->handler_index;
  interaction->handler_elapsed[elapsed_index] = 0;
  interaction->attack_target = target;
}

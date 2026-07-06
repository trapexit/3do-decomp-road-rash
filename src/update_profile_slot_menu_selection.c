/* Reconstructed from the original function at 0x00007AAC. */

#include "front_end_commands.h"

void
draw_saved_profile_slots(int selected_slot);

void
update_profile_slot_menu_selection(int command)
{
  int selected_slot;

  if(command >= FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST &&
     command <= FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST +
     PROFILE_SLOT_EXIT)
    {
      selected_slot = command - FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST;
    }
  else if(command >= FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST &&
          command <= FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST +
          PROFILE_SLOT_EXIT)
    {
      selected_slot = command - FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST;
    }
  else if(command >= FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST &&
          command <= FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST +
          PROFILE_SLOT_EXIT)
    {
      selected_slot = command - FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST;
    }
  else
    {
      return;
    }

  draw_saved_profile_slots(selected_slot);
}

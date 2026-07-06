#include "front_end_commands.h"
#include "rw_semantic_data.h"

/* Reconstructed from the original function at 0x00007BA8. */



FrontEndMenu *
find_front_end_menu(int menu_id);
void
draw_saved_profile_slots(int selected_slot);

void
draw_profile_slot_menu(int menu_id,
                       int initial_selection)
{
  FrontEndMenu *menu;
  int first_slot_command;
  int selected_slot;

  if(menu_id == FRONT_END_MENU_PROFILE_GROUP_A)
    {
      first_slot_command = FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST;
    }
  else if(menu_id == FRONT_END_MENU_PROFILE_GROUP_B)
    {
      first_slot_command = FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST;
    }
  else if(menu_id == FRONT_END_MENU_PROFILE_GROUP_C)
    {
      first_slot_command = FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST;
    }
  else
    {
      return;
    }

  selected_slot = initial_selection & FRONT_END_SELECTION_MASK;
  if(selected_slot == PROFILE_SLOT_ONE)
    {
      menu = find_front_end_menu(menu_id);
      selected_slot = menu->current_action_id - first_slot_command;
    }
  draw_saved_profile_slots(selected_slot);
}


void
select_active_profile_player(void)
{
  if(race_mode_state.player_count == FRONT_END_TWO_PLAYERS)
    {
      race_mode_state.active_player_index =
        (signed char)(player_profiles[1].rider_type ==
                      RIDER_TYPE_PLAYER_TWO);
    }
}

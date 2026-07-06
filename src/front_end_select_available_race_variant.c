#include "front_end_commands.h"
#include "rw_semantic_data.h"

FrontEndMenu *
find_front_end_menu(int menu_id);

int
front_end_select_available_race_variant(void)
{
  FrontEndMenu *menu;
  RiderProfile *profile;
  int index;
  int mask;
  int tries;

  if(front_end_preview_state.operation_busy != 0)
    {
      return front_end_preview_state.operation_busy;
    }

  index = race_mode_state.active_player_index;
  profile = &player_profiles[index];
  mask = profile->completed_courses;
  index = profile->current_variant;

  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      menu = find_front_end_menu(FRONT_END_MENU_DEFAULT_VARIANT);
    }
  else
    {
      menu = find_front_end_menu(FRONT_END_MENU_ALTERNATE_RIDER_SETUP);
    }

  if(race_input_latch_state.event_latched != 0)
    {
      index++;
      if(index >= RIDER_VARIANT_COUNT)
        {
          index = 0;
        }
    }

  tries = 0;
  while((mask & (1 << index)) != 0 &&
        tries < RIDER_VARIANT_COUNT)
    {
      index++;
      if(index >= RIDER_VARIANT_COUNT)
        {
          index = 0;
        }
      tries++;
    }

  if(race_mode_state.game_mode != FRONT_END_GAME_MODE_STANDARD)
    {
      menu->current_action_id =
        index + FRONT_END_COMMAND_SELECT_VARIANT_B_FIRST;
    }
  else
    {
      menu->current_action_id =
        index + FRONT_END_COMMAND_SELECT_VARIANT_A_FIRST;
    }
  return (int)&menu->current_action_id;
}

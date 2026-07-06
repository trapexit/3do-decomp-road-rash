/* Reconstructed from the original function at 0x00007D74 using Ghidra and assembly. */



#include "front_end_commands.h"
#include "rw_semantic_data.h"

void
initialize_rider_progression_profiles(void);
void
select_active_profile_player(void);
void
reset_player_profiles(int reset_player_one,
                      int reset_player_two,
                      int reset_rider_type);
FrontEndAction *
find_front_end_action(int action);
int
scan_saved_profile_slots(void);
int
save_rider_profile_slot(int profile_slot,
                        int player_index);
int
load_rider_profile_slot(int profile_slot,
                        int player_index);
void
select_rider_opponents(int player_index);

static
RiderProfile *
current_rider_profile(void)
{
  return &player_profiles[race_mode_state.active_player_index];
}


static
void
set_front_end_game_mode(int mode)
{
  if(race_mode_state.game_mode == mode)
    {
      return;
    }

  reset_player_profiles(1, 1, 1);
  initialize_rider_progression_profiles();
  race_mode_state.game_mode = (unsigned char)mode;
  front_end_preview_state.next_menu_override_id =
    mode != 0 ? FRONT_END_MENU_ALTERNATE_RIDER_SETUP :
    FRONT_END_MENU_DEFAULT_RIDER_SETUP;
}


static
void
set_front_end_player_count(int count)
{
  signed char first;
  signed char second;

  race_mode_state.player_count = (unsigned char)count;
  first = player_profiles[0].rider_type;

  if(count == FRONT_END_SINGLE_PLAYER)
    {
      race_mode_state.active_player_index = 0;
      if(race_mode_state.game_mode == 0 && first == 0)
        {
          front_end_preview_state.next_menu_override_id =
            FRONT_END_MENU_DEFAULT_RIDER_SETUP;
        }
      return;
    }

  second = player_profiles[1].rider_type;
  if(race_mode_state.game_mode == 0 &&
     (first == RIDER_TYPE_PLAYER_ONE ||
      second == RIDER_TYPE_PLAYER_TWO))
    {
      front_end_preview_state.next_menu_override_id =
        FRONT_END_MENU_DEFAULT_RIDER_SETUP;
    }
}


static
void
copy_next_menu_override(int action)
{
  FrontEndAction *entry;
  int next;

  entry = find_front_end_action(action);
  next = front_end_preview_state.next_menu_override_id;
  entry->success_menu_id = next;
  entry->failure_menu_id = next;
}


static
void
prepare_profile_slot_operation(int operation_state)
{
  scan_saved_profile_slots();
  front_end_preview_state.profile_operation_state = operation_state;
}


static
void
apply_profile_slot(int slot)
{
  int player;

  player = front_end_preview_state.profile_player_index;
  if(front_end_preview_state.profile_operation_state == 0)
    {
      save_rider_profile_slot(slot, player);
      return;
    }

  if(load_rider_profile_slot(slot, player) != 0)
    {
      front_end_preview_state.profile_operation_state = -1;
    }

  if(race_mode_state.player_count == FRONT_END_TWO_PLAYERS)
    {
      if(player_profiles[0].rider_type != 0 &&
         player_profiles[1].rider_type != RIDER_TYPE_PLAYER_TWO)
        {
          front_end_preview_state.next_menu_override_id =
            FRONT_END_MENU_RIDER_SETUP_COMPLETE;
        }
    }
  else if(race_mode_state.player_count == FRONT_END_SINGLE_PLAYER &&
          player_profiles[0].rider_type != 0)
    {
      front_end_preview_state.next_menu_override_id =
        FRONT_END_MENU_RIDER_SETUP_COMPLETE;
    }
}


static
void
purchase_rider_style(int style)
{
  RiderProfile *rider;
  int old_style;
  int delta;

  rider = current_rider_profile();
  old_style = rider->rider_style;
  delta = rider_style_purchase_prices[style] -
          rider_style_resale_values[old_style];
  rider->balance -= delta;
  rider->rider_style = (signed char)style;
}


static
void
select_rider_type(int type)
{
  RiderProfile *rider;
  int index;

  index = race_mode_state.active_player_index;
  rider = &player_profiles[index];
  rider->balance = rider_starting_balances[type];
  rider->rider_type = (signed char)type;
  select_rider_opponents(index);
  rider->rider_style =
    (signed char)rider_style_defaults[rider->rider_type][0];
}


static
void
select_rider_variant(int variant)
{
  current_rider_profile()->current_variant = (signed char)variant;
}


void
handle_front_end_command(int command)
{
  switch(command)
    {
    case FRONT_END_COMMAND_SHOW_ALTERNATE_SETUP:
      front_end_preview_state.next_menu_override_id =
        FRONT_END_MENU_ALTERNATE_RIDER_SETUP;
      return;

    case FRONT_END_COMMAND_SET_STANDARD_MODE_A:
    case FRONT_END_COMMAND_SET_STANDARD_MODE_B:
    case FRONT_END_COMMAND_SET_STANDARD_MODE_C:
    case FRONT_END_COMMAND_SET_STANDARD_MODE_D:
      set_front_end_game_mode(FRONT_END_GAME_MODE_STANDARD);
      return;

    case FRONT_END_COMMAND_SET_ALTERNATE_MODE_A:
    case FRONT_END_COMMAND_SET_ALTERNATE_MODE_B:
    case FRONT_END_COMMAND_SET_ALTERNATE_MODE_C:
    case FRONT_END_COMMAND_SET_ALTERNATE_MODE_D:
      set_front_end_game_mode(FRONT_END_GAME_MODE_ALTERNATE);
      return;

    case FRONT_END_COMMAND_SCAN_LOAD_SLOTS_A:
    case FRONT_END_COMMAND_SCAN_LOAD_SLOTS_B:
    case FRONT_END_COMMAND_SCAN_LOAD_SLOTS_C:
      prepare_profile_slot_operation(1);
      return;

    case FRONT_END_COMMAND_SCAN_SAVE_SLOTS:
      prepare_profile_slot_operation(0);
      return;

    case FRONT_END_COMMAND_COPY_MENU_OVERRIDE_A:
    case FRONT_END_COMMAND_COPY_MENU_OVERRIDE_B:
    case FRONT_END_COMMAND_COPY_MENU_OVERRIDE_C:
      copy_next_menu_override(command);
      return;

    case FRONT_END_COMMAND_SET_SINGLE_PLAYER_A:
    case FRONT_END_COMMAND_SET_SINGLE_PLAYER_B:
    case FRONT_END_COMMAND_SET_SINGLE_PLAYER_C:
      set_front_end_player_count(FRONT_END_SINGLE_PLAYER);
      return;

    case FRONT_END_COMMAND_SET_TWO_PLAYERS_A:
    case FRONT_END_COMMAND_SET_TWO_PLAYERS_B:
    case FRONT_END_COMMAND_SET_TWO_PLAYERS_C:
      set_front_end_player_count(FRONT_END_TWO_PLAYERS);
      return;

    case FRONT_END_COMMAND_ENABLE_ENGINE_AUDIO_A:
    case FRONT_END_COMMAND_ENABLE_ENGINE_AUDIO_B:
    case FRONT_END_COMMAND_ENABLE_ENGINE_AUDIO_C:
      race_mode_state.engine_audio_enabled = 1;
      return;

    case FRONT_END_COMMAND_DISABLE_ENGINE_AUDIO_A:
    case FRONT_END_COMMAND_DISABLE_ENGINE_AUDIO_B:
    case FRONT_END_COMMAND_DISABLE_ENGINE_AUDIO_C:
      race_mode_state.engine_audio_enabled = 0;
      return;

    case FRONT_END_COMMAND_ENABLE_MUSIC_A:
    case FRONT_END_COMMAND_ENABLE_MUSIC_B:
    case FRONT_END_COMMAND_ENABLE_MUSIC_C:
      race_mode_state.music_enabled = 1;
      return;

    case FRONT_END_COMMAND_DISABLE_MUSIC_A:
    case FRONT_END_COMMAND_DISABLE_MUSIC_B:
    case FRONT_END_COMMAND_DISABLE_MUSIC_C:
      race_mode_state.music_enabled = 0;
      return;

    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_ONE:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_ONE:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_ONE:
      apply_profile_slot(PROFILE_SLOT_ONE);
      return;
    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_TWO:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_TWO:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_TWO:
      apply_profile_slot(PROFILE_SLOT_TWO);
      return;
    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_THREE:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_THREE:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_THREE:
      apply_profile_slot(PROFILE_SLOT_THREE);
      return;
    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_FOUR:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_FOUR:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_FOUR:
      apply_profile_slot(PROFILE_SLOT_FOUR);
      return;
    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_FIVE:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_FIVE:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_FIVE:
      apply_profile_slot(PROFILE_SLOT_FIVE);
      return;
    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_SIX:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_SIX:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_SIX:
      apply_profile_slot(PROFILE_SLOT_SIX);
      return;
    case FRONT_END_COMMAND_PROFILE_GROUP_A_FIRST + PROFILE_SLOT_SEVEN:
    case FRONT_END_COMMAND_PROFILE_GROUP_B_FIRST + PROFILE_SLOT_SEVEN:
    case FRONT_END_COMMAND_PROFILE_GROUP_C_FIRST + PROFILE_SLOT_SEVEN:
      apply_profile_slot(PROFILE_SLOT_SEVEN);
      return;

    case FRONT_END_COMMAND_SELECT_RIDER_FIRST:
      select_rider_type(RIDER_TYPE_MIKE);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_CYDNEY - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_CYDNEY);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_JIM - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_JIM);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_PEARL - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_PEARL);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_JON - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_JON);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_RHONDA - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_RHONDA);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_BOSE - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_BOSE);
      select_active_profile_player();
      return;
    case FRONT_END_COMMAND_SELECT_RIDER_FIRST +
      (RIDER_TYPE_AXLE - RIDER_TYPE_MIKE):
      select_rider_type(RIDER_TYPE_AXLE);
      select_active_profile_player();
      return;

    case FRONT_END_COMMAND_SELECT_VARIANT_A_FIRST + RIDER_VARIANT_PRIMARY:
    case FRONT_END_COMMAND_SELECT_VARIANT_B_FIRST + RIDER_VARIANT_PRIMARY:
      select_rider_variant(RIDER_VARIANT_PRIMARY);
      return;
    case FRONT_END_COMMAND_SELECT_VARIANT_A_FIRST + RIDER_VARIANT_ONE:
    case FRONT_END_COMMAND_SELECT_VARIANT_B_FIRST + RIDER_VARIANT_ONE:
      select_rider_variant(RIDER_VARIANT_ONE);
      return;
    case FRONT_END_COMMAND_SELECT_VARIANT_A_FIRST + RIDER_VARIANT_TWO:
    case FRONT_END_COMMAND_SELECT_VARIANT_B_FIRST + RIDER_VARIANT_TWO:
      select_rider_variant(RIDER_VARIANT_TWO);
      return;
    case FRONT_END_COMMAND_SELECT_VARIANT_A_FIRST + RIDER_VARIANT_THREE:
    case FRONT_END_COMMAND_SELECT_VARIANT_B_FIRST + RIDER_VARIANT_THREE:
      select_rider_variant(RIDER_VARIANT_THREE);
      return;
    case FRONT_END_COMMAND_SELECT_VARIANT_A_FIRST + RIDER_VARIANT_FOUR:
    case FRONT_END_COMMAND_SELECT_VARIANT_B_FIRST + RIDER_VARIANT_FOUR:
      select_rider_variant(RIDER_VARIANT_FOUR);
      return;
    case FRONT_END_COMMAND_SHOW_ALTERNATE_SETUP_AGAIN:
      front_end_preview_state.next_menu_override_id =
        FRONT_END_MENU_ALTERNATE_RIDER_SETUP;
      return;

    case FRONT_END_COMMAND_SELECT_STYLE_PICO:
      purchase_rider_style(RIDER_STYLE_PICO);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_SPORT_450:
      purchase_rider_style(RIDER_STYLE_SPORT_450);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_SWALLOW:
      purchase_rider_style(RIDER_STYLE_SWALLOW);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_KILLER:
      purchase_rider_style(RIDER_STYLE_KILLER);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_ZYX_250:
      purchase_rider_style(RIDER_STYLE_ZYX_250);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_AGGRESSORE:
      purchase_rider_style(RIDER_STYLE_AGGRESSORE);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_VAMPIRO:
      purchase_rider_style(RIDER_STYLE_VAMPIRO);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_GRANDE:
      purchase_rider_style(RIDER_STYLE_GRANDE);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_M10:
      purchase_rider_style(RIDER_STYLE_M10);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_ZYX_750:
      purchase_rider_style(RIDER_STYLE_ZYX_750);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_ASSASSINO:
      purchase_rider_style(RIDER_STYLE_ASSASSINO);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_VIPERA_N:
      purchase_rider_style(RIDER_STYLE_VIPERA_N);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_RAVEN_N:
      purchase_rider_style(RIDER_STYLE_RAVEN_N);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_711_SS1:
      purchase_rider_style(RIDER_STYLE_711_SS1);
      return;
    case FRONT_END_COMMAND_SELECT_STYLE_ZYX_750N:
      purchase_rider_style(RIDER_STYLE_ZYX_750N);
      return;
    case FRONT_END_COMMAND_COMPLETE_RIDER_SETUP:
      front_end_preview_state.next_menu_override_id =
        FRONT_END_MENU_RIDER_SETUP_COMPLETE;
      return;
    default:
      return;
    }
}

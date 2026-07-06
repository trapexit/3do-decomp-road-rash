#include "front_end_commands.h"
#include "rw_semantic_data.h"

/* Reconstructed from the original function at 0x000074C0. */



void
toggle_active_front_end_player(void)
{
  if(race_mode_state.player_count != FRONT_END_TWO_PLAYERS)
    {
      race_mode_state.active_player_index = 0;
      return;
    }

  race_mode_state.active_player_index =
    (signed char)(1 - race_mode_state.active_player_index);
}


void
toggle_front_end_profile_player(void)
{
  if(race_mode_state.player_count != FRONT_END_TWO_PLAYERS)
    {
      front_end_preview_state.profile_player_index = 0;
      return;
    }

  front_end_preview_state.profile_player_index =
    1 - front_end_preview_state.profile_player_index;
}

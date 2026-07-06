#include "front_end_cel_runtime.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"

void
front_end_synchronize_current_player(void)
{
  FrontEndCelDescriptor *selected;

  if(race_mode_state.player_count <= FRONT_END_SINGLE_PLAYER)
    {
      return;
    }

  race_mode_state.active_player_index = (signed char)(
    race_mode_state.game_mode != FRONT_END_GAME_MODE_STANDARD);
  if(race_mode_state.game_mode != FRONT_END_GAME_MODE_STANDARD)
    {
      selected = front_end_cel_group_006;
    }
  else
    {
      selected = front_end_cel_group_005;
    }
  apply_front_end_cel_descriptors(selected);
}

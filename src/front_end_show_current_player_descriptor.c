#include "front_end_cel_runtime.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"

void
front_end_show_current_player_descriptor(void)
{
  FrontEndCelDescriptor *selected;

  if(race_mode_state.player_count <= FRONT_END_SINGLE_PLAYER)
    {
      return;
    }

  if(race_mode_state.active_player_index == 0)
    {
      selected = front_end_cel_group_005;
    }
  else if(race_mode_state.active_player_index == 1)
    {
      selected = front_end_cel_group_006;
    }
  else
    {
      return;
    }
  apply_front_end_cel_descriptors(selected);
}

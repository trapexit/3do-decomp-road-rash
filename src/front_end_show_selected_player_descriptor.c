#include "front_end_cel_runtime.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"

void
front_end_show_selected_player_descriptor(void)
{
  FrontEndCelDescriptor *selected;

  if(race_mode_state.player_count <= FRONT_END_SINGLE_PLAYER)
    {
      return;
    }

  if(front_end_preview_state.profile_player_index != 0)
    {
      selected = front_end_cel_group_006;
    }
  else
    {
      selected = front_end_cel_group_005;
    }
  apply_front_end_cel_descriptors(selected);
}

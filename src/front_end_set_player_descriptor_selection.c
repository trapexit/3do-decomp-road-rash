#include "rw_semantic_data.h"

int
front_end_set_player_descriptor_selection(int value)
{
  front_end_preview_state.profile_player_index = value;
  return 1;
}

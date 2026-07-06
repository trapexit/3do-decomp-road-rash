#include "front_end_commands.h"
#include "rw_semantic_data.h"

int
front_end_is_player_configuration_valid(void)
{
  if(race_mode_state.player_count == FRONT_END_SINGLE_PLAYER)
    {
      return 1;
    }

  if(player_profiles[0].rider_type == RIDER_TYPE_PLAYER_ONE &&
     player_profiles[1].rider_type == RIDER_TYPE_PLAYER_TWO)
    {
      return 0;
    }
  return 1;
}

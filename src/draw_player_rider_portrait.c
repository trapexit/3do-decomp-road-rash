#include "display_cel_runtime.h"
#include "hud_runtime.h"
#include "rw_semantic_data.h"

enum PlayerRiderPortraitConstant
{
  PLAYER_RIDER_PORTRAIT_COORDINATE_BASE = 7
};

int32 *
draw_player_rider_portrait(void)
{
  CCB *portrait;
  HudCoordinate *position;
  int player_index;
  int rider_type;
  int layout;

  player_index = race_mode_state.active_player_index;
  rider_type = player_profiles[player_index].rider_type;

  if(rider_type > 0 && race_mode_state.game_mode == 0)
    {
      portrait = (CCB *)rider_resource_items[rider_type];
    }
  else
    {
      portrait = (CCB *)hud_globals.rider_resource_items[player_index];
    }

  if(portrait == 0)
    {
      return 0;
    }

  portrait = append_unmapped_cel(portrait);
  layout = runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals.coordinates[
    PLAYER_RIDER_PORTRAIT_COORDINATE_BASE + layout];
  return map_cel_at_native_scale(
    portrait, position->x, position->y);
}

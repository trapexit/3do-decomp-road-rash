#include "hud_runtime.h"

void
draw_player_race_status_panel(void)
{
  draw_player_rider_health_gauge();
  draw_player_bike_health_gauge();
  draw_player_rider_portrait();
  draw_player_race_position_readout();
}

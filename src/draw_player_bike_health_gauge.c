#include "division_helpers.h"
#include "hud_runtime.h"
#include "racer_runtime.h"

enum
{
  HUD_LAYOUT_BYTE_MASK = 0xFF,
  HUD_HEALTH_ANIMATION_LAST_FRAME = 31,
  BIKE_HEALTH_ANGLE_SPAN = 64,
  BIKE_HEALTH_ANGLE_OFFSET = 30,
  PLAYER_BIKE_HEALTH_COORDINATE_INDEX = 3,
  BIKE_HEALTH_NEEDLE_PIVOT_X = 17,
  BIKE_HEALTH_NEEDLE_PIVOT_Y = 1
};

void
draw_player_bike_health_gauge(void)
{
  RacerEntity *player;
  signed char layout_index;
  HudCoordinate *position;
  int gauge_value;

  layout_index =
    (signed char)runtime_feature_flags.alternate_hud_layout.value;
  player = (RacerEntity *)race_rider_state.player_object;
  gauge_value = player->current_bike_health;
  position = &hud_globals.coordinates[
    PLAYER_BIKE_HEALTH_COORDINATE_INDEX + layout_index];
  if((layout_index & HUD_LAYOUT_BYTE_MASK) != 0)
    {
      gauge_value = signed_divide_with_remainder(
        (unsigned int)player->maximum_bike_health,
        (unsigned int)(gauge_value * HUD_HEALTH_ANIMATION_LAST_FRAME))
                    .quotient;
      draw_hud_health_gauge_animation_frame(gauge_value, position);
      return;
    }

  gauge_value = signed_divide_with_remainder(
    (unsigned int)player->maximum_bike_health,
    (unsigned int)(gauge_value * BIKE_HEALTH_ANGLE_SPAN)).quotient +
                BIKE_HEALTH_ANGLE_OFFSET;
  draw_hud_health_gauge_needle(
    gauge_value, position->x, position->y,
    BIKE_HEALTH_NEEDLE_PIVOT_X, BIKE_HEALTH_NEEDLE_PIVOT_Y);
}

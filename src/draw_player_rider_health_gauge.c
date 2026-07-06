#include "division_helpers.h"
#include "hud_runtime.h"
#include "racer_runtime.h"

enum
{
  HUD_LAYOUT_BYTE_MASK = 0xFF,
  HUD_HEALTH_ANIMATION_LAST_FRAME = 31,
  HUD_HEALTH_HALF_TURN_ANGLE = 128,
  PLAYER_RIDER_HEALTH_COORDINATE_INDEX = 1,
  RIDER_HEALTH_NEEDLE_PIVOT_X = 8,
  RIDER_HEALTH_NEEDLE_PIVOT_Y = 1
};

void
draw_player_rider_health_gauge(void)
{
  RacerEntity *player;
  RiderRuntimeSlot *runtime;
  signed char layout_index;
  HudCoordinate *position;
  int gauge_value;

  layout_index =
    (signed char)runtime_feature_flags.alternate_hud_layout.value;
  player = (RacerEntity *)race_rider_state.player_object;
  runtime = player->paired_state->runtime;

  gauge_value = runtime->current_health;
  position = &hud_globals.coordinates[
    PLAYER_RIDER_HEALTH_COORDINATE_INDEX + layout_index];
  if((layout_index & HUD_LAYOUT_BYTE_MASK) != 0)
    {
      gauge_value = signed_divide_with_remainder(
        (unsigned int)runtime->maximum_health,
        (unsigned int)(gauge_value * HUD_HEALTH_ANIMATION_LAST_FRAME))
                    .quotient;
      draw_hud_health_gauge_animation_frame(gauge_value, position);
      return;
    }

  gauge_value = signed_divide_with_remainder(
    (unsigned int)runtime->maximum_health,
    (unsigned int)(gauge_value * HUD_HEALTH_HALF_TURN_ANGLE)).quotient;
  draw_hud_health_gauge_needle(
    gauge_value, position->x, position->y,
    RIDER_HEALTH_NEEDLE_PIVOT_X, RIDER_HEALTH_NEEDLE_PIVOT_Y);
}

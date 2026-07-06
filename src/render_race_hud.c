#include "display_cel_runtime.h"
#include "hud_runtime.h"
#include "rw_semantic_data.h"
#include "text_render_runtime.h"

enum RaceHudRenderConstant
{
  RACE_HUD_PRIMARY_COORDINATE_INDEX = 0,
  RACE_HUD_CEL_DIMENSION_MASK = 0x1FF,
  RACE_HUD_CEL_DIMENSION_EXPONENT_MASK = 0x1F,
  RACE_HUD_STANDARD_TEXT_COMPONENT = 0x10
};

static
int
decode_race_hud_cel_dimension(int field)
{
  if(field < 0)
    {
      return 1 << (field & RACE_HUD_CEL_DIMENSION_EXPONENT_MASK);
    }
  return field & RACE_HUD_CEL_DIMENSION_MASK;
}


void
render_race_hud(void)
{
  HudCoordinate *position;
  CCB *packet;
  int packet_height;
  int alternate_layout;

  alternate_layout =
    runtime_feature_flags.alternate_hud_layout.value;
  if(alternate_layout == 0)
    {
      packet = append_unmapped_cel(hud_globals.primary_hud_cel);
      packet_height =
        decode_race_hud_cel_dimension(packet->ccb_Height);
      position = &hud_globals
                 .coordinates[RACE_HUD_PRIMARY_COORDINATE_INDEX];
      map_cel_at_native_scale(
        packet, position->x, position->y - packet_height);
    }

  set_text_color(TEXT_COLOR_COMPONENT_MAX,
                 TEXT_COLOR_COMPONENT_MAX,
                 TEXT_COLOR_COMPONENT_MAX);
  draw_player_race_status_panel();
  draw_tracked_rider_health_gauge();

  if(alternate_layout == 0)
    {
      set_text_color(RACE_HUD_STANDARD_TEXT_COMPONENT,
                     RACE_HUD_STANDARD_TEXT_COMPONENT,
                     RACE_HUD_STANDARD_TEXT_COMPONENT);
    }

  draw_player_progress_readout();
  draw_race_boost_charge_bar();

  if(alternate_layout != 0)
    {
      draw_player_speed_readout();
      return;
    }

  draw_player_speedometer_needle();
  draw_player_engine_pitch_needle();
}

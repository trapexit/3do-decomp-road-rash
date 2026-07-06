#include "display_cel_runtime.h"
#include "hud_runtime.h"

enum
{
  HUD_HEALTH_GAUGE_MIN_ANGLE = 0,
  HUD_HEALTH_GAUGE_MAX_ANGLE = 128
};

void
draw_hud_health_gauge_needle(int angle_256ths,
                             int anchor_x,
                             int anchor_y,
                             int pivot_x,
                             int pivot_y)
{
  int clamped_angle;

  clamped_angle = angle_256ths;
  if(clamped_angle < HUD_HEALTH_GAUGE_MIN_ANGLE)
    {
      clamped_angle = HUD_HEALTH_GAUGE_MIN_ANGLE;
    }
  else if(clamped_angle > HUD_HEALTH_GAUGE_MAX_ANGLE)
    {
      clamped_angle = HUD_HEALTH_GAUGE_MAX_ANGLE;
    }

  map_rotated_cel_about_pivot(
    append_unmapped_cel(hud_globals.health_gauge_needle_ccb),
    clamped_angle, anchor_x, anchor_y, pivot_x, pivot_y);
}

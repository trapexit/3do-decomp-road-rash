#include "display_cel_runtime.h"
#include "hud_runtime.h"

enum
{
  SPEEDOMETER_MAX_ANGLE = 250,
  SPEEDOMETER_NEEDLE_PIVOT_X = 33,
  SPEEDOMETER_NEEDLE_PIVOT_Y = 3
};

static const HudGlobals *const hud_globals_reference_for_gauge_needles =
  &hud_globals;

void
draw_speedometer_needle(int angle_256ths,
                        int anchor_x,
                        int anchor_y)
{
  int clamped_angle;

  clamped_angle = angle_256ths;
  if(clamped_angle > SPEEDOMETER_MAX_ANGLE)
    {
      clamped_angle = SPEEDOMETER_MAX_ANGLE;
    }

  map_rotated_cel_about_pivot(
    append_unmapped_cel(
      hud_globals_reference_for_gauge_needles->speedometer_needle_ccb),
    clamped_angle, anchor_x, anchor_y,
    SPEEDOMETER_NEEDLE_PIVOT_X, SPEEDOMETER_NEEDLE_PIVOT_Y);
}

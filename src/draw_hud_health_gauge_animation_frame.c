#include "display_cel_runtime.h"
#include "front_end_cel_runtime.h"
#include "hud_runtime.h"

void
draw_hud_health_gauge_animation_frame(int                  frame_index,
                                      const HudCoordinate *position)
{
  CansAnimationFrameOutput animation_frame;
  CCB *cel;

  CANS_GetAnimFrame((CansAnimationData *)hud_globals.gauge_animation_item,
                   frame_index,
                   &animation_frame);
  cel = append_unmapped_cel(animation_frame.ccb);
  map_cel_at_native_scale(cel, position->x, position->y);
}

#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "front_end_cel_runtime.h"
#include "hud_runtime.h"
#include "racer_runtime.h"

enum
{
  HUD_LAYOUT_BYTE_MASK = 0xFF,
  HUD_HEALTH_ANIMATION_LAST_FRAME = 31,
  HUD_HEALTH_HALF_TURN_ANGLE = 128,
  TRACKED_RIDER_HEALTH_COORDINATE_INDEX = 5,
  TRACKED_RIDER_ICON_COORDINATE_INDEX = 11,
  RIDER_HEALTH_NEEDLE_PIVOT_X = 8,
  RIDER_HEALTH_NEEDLE_PIVOT_Y = 1
};

static
int
decode_hud_cel_dimension(int encoded_dimension)
{
  if(encoded_dimension < 0)
    {
      return (int)(1U <<
                   (encoded_dimension & CANS_CCB_DIMENSION_EXPONENT_MASK));
    }
  return encoded_dimension & CANS_CCB_DIMENSION_LITERAL_MASK;
}


void
draw_tracked_rider_health_gauge(void)
{
  RacerEntity *tracked_rider;
  RiderRuntimeSlot *runtime;
  signed char layout_index;
  HudCoordinate *position;
  CCB *icon_cel;
  int icon_width;
  int gauge_value;

  tracked_rider = (RacerEntity *)hud_globals.tracked_object;
  if(tracked_rider == 0)
    {
      return;
    }

  runtime = tracked_rider->runtime;
  if(runtime->resource_item != 0)
    {
      icon_cel = append_unmapped_cel((const CCB *)runtime->resource_item);
      layout_index =
        (signed char)runtime_feature_flags.alternate_hud_layout.value;
      position = &hud_globals.coordinates[
        TRACKED_RIDER_ICON_COORDINATE_INDEX + layout_index];
      icon_width = decode_hud_cel_dimension(icon_cel->ccb_Width);
      map_cel_at_native_scale(icon_cel,
                              position->x - icon_width, position->y);
    }

  layout_index =
    (signed char)runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals.coordinates[
    TRACKED_RIDER_HEALTH_COORDINATE_INDEX + layout_index];
  gauge_value = runtime->current_health;
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

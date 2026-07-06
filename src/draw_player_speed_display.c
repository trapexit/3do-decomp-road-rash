#include "division_helpers.h"
#include "hud_runtime.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

#include "stdio.h"
#include "string.h"

enum PlayerSpeedDisplayConstant
{
  PLAYER_SPEED_PRIMARY_SHIFT = 7,
  PLAYER_SPEED_SECONDARY_SHIFT = 9,
  PLAYER_SPEED_TERTIARY_SHIFT = 12,
  PLAYER_SPEED_NEEDLE_MINIMUM = 0x32,
  PLAYER_SPEED_NEEDLE_MAXIMUM = 0xB4,
  PLAYER_SPEED_NEEDLE_DIVISOR = 0x5A,
  PLAYER_SPEED_NEEDLE_SCALE_SHIFT = 7,
  PLAYER_SPEED_NEEDLE_BIAS = 0x2300,
  PLAYER_SPEED_COORDINATE_BASE = 13,
  PLAYER_SPEED_TEXT_SPACING = 2,
  PLAYER_SPEED_TEXT_ALIGNMENT = 4
};

// A zero byte count marks a cold cache; text includes its terminating NUL.
typedef struct PlayerSpeedTextCache
{
  s32 _value;
  size_t _text_bytes;
  char _text[sizeof("-2147483648")];
} PlayerSpeedTextCache;

static PlayerSpeedTextCache g_PLAYER_SPEED_TEXT_CACHE;

int
draw_text(int bitmap_item,
          int x,
          int y,
          int text_address,
          int character_spacing,
          int alignment);

static
int
player_display_speed(void)
{
  RacerEntity *player;
  int velocity;

  player = (RacerEntity *)race_rider_state.player_object;
  velocity = player->velocity.forward;
  return (velocity >> PLAYER_SPEED_PRIMARY_SHIFT) +
         (velocity >> PLAYER_SPEED_SECONDARY_SHIFT) +
         (velocity >> PLAYER_SPEED_TERTIARY_SHIFT);
}


void
draw_player_speedometer_needle(void)
{
  SignedDivisionResult division;
  HudCoordinate *position;
  int speed;
  int layout;

  speed = player_display_speed();
  if(speed < PLAYER_SPEED_NEEDLE_MINIMUM)
    {
      speed = PLAYER_SPEED_NEEDLE_MINIMUM;
    }
  else if(speed > PLAYER_SPEED_NEEDLE_MAXIMUM)
    {
      speed = PLAYER_SPEED_NEEDLE_MAXIMUM;
    }

  division = signed_divide_with_remainder(
    PLAYER_SPEED_NEEDLE_DIVISOR,
    (unsigned int)((speed << PLAYER_SPEED_NEEDLE_SCALE_SHIFT) -
                   PLAYER_SPEED_NEEDLE_BIAS));

  layout = runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals
             .coordinates[PLAYER_SPEED_COORDINATE_BASE + layout];
  draw_speedometer_needle(
    division.quotient, position->x, position->y);
}


void
draw_player_speed_readout(void)
{
  HudCoordinate *position;
  int speed;
  int layout;

  speed = player_display_speed();
  if(speed < 0)
    {
      speed = 0;
    }

  if((g_PLAYER_SPEED_TEXT_CACHE._text_bytes == 0) ||
     (g_PLAYER_SPEED_TEXT_CACHE._value != speed))
    {
      g_PLAYER_SPEED_TEXT_CACHE._text_bytes =
        (size_t)(sprintf(hud_globals.formatted_text, "%ld", speed) + 1);
      g_PLAYER_SPEED_TEXT_CACHE._value = speed;
      memcpy(g_PLAYER_SPEED_TEXT_CACHE._text,
             hud_globals.formatted_text,
             g_PLAYER_SPEED_TEXT_CACHE._text_bytes);
    }
  else
    {
      // Other readouts share this buffer; do not overwrite bytes after the NUL.
      memcpy(hud_globals.formatted_text,
             g_PLAYER_SPEED_TEXT_CACHE._text,
             g_PLAYER_SPEED_TEXT_CACHE._text_bytes);
    }

  layout = runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals
             .coordinates[PLAYER_SPEED_COORDINATE_BASE + layout];
  draw_text(
    display_runtime_globals
    .screen_items[display_runtime_globals.active_bitmap_index],
    position->x, position->y, (int)hud_globals.formatted_text,
    PLAYER_SPEED_TEXT_SPACING, PLAYER_SPEED_TEXT_ALIGNMENT);
}

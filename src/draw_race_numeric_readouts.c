#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "hud_runtime.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

#include "stdio.h"
#include "string.h"

enum RaceNumericReadoutConstant
{
  RACE_PROGRESS_FRACTION_SHIFT = 8,
  RACE_PROGRESS_ORIGIN = 0x32,
  RACE_PROGRESS_UNIT_SPAN = 0x21,
  RACE_PROGRESS_DIGIT_COUNT = 3,
  RACE_PROGRESS_BACKGROUND_LEFT = 0xD5,
  RACE_PROGRESS_BACKGROUND_TOP = 0xDB,
  RACE_PROGRESS_BACKGROUND_RIGHT = 0xD7,
  RACE_PROGRESS_BACKGROUND_BOTTOM = 0xDD,
  RACE_PROGRESS_BACKGROUND_COLOR = 0x6318,
  RACE_PROGRESS_COORDINATE_BASE = 17,
  ENGINE_PITCH_GAUGE_ORIGIN = 0x3000,
  ENGINE_PITCH_GAUGE_STEP = 0x5000,
  ENGINE_PITCH_GAUGE_MINIMUM = -0x1000,
  ENGINE_PITCH_GAUGE_ANGLE_SHIFT = 9,
  ENGINE_PITCH_COORDINATE_BASE = 15,
  RACER_FINISHED_FLAG = 0x10,
  RACE_POSITION_STATUS_LOW_MASK = 0x00FFFFFF,
  RACE_POSITION_STATUS_SHIFT = 24,
  RACE_POSITION_ALTERNATE_COORDINATE_BASE = 9,
  RACE_POSITION_STANDARD_COORDINATE_BASE = 19,
  HUD_TEXT_CHARACTER_SPACING = 2,
  HUD_TEXT_ALIGNMENT_LEFT = 2,
  HUD_TEXT_ALIGNMENT_RIGHT = 4
};

// Each readout owns its padded text; zero bytes marks a cold cache.
typedef struct RaceNumericTextCache
{
  s32 _value;
  size_t _text_bytes;
  char _text[sizeof("-2147483648")];
} RaceNumericTextCache;

static RaceNumericTextCache g_RACE_PROGRESS_TEXT_CACHE;
static RaceNumericTextCache g_RACE_POSITION_TEXT_CACHE;

int
draw_text(int bitmap_item,
          int x,
          int y,
          int text_address,
          int character_spacing,
          int alignment);

static
int
current_display_bitmap_item(void)
{
  return display_runtime_globals
         .screen_items[display_runtime_globals.active_bitmap_index];
}


static
int
player_track_progress_units(void)
{
  RacerEntity *player;

  player = (RacerEntity *)race_rider_state.player_object;
  return player->position_snapshot.position_z >>
         RACE_PROGRESS_FRACTION_SHIFT;
}


static
void
replace_leading_spaces_with_zeroes(char *text)
{
  int digit;

  digit = 0;
  while(digit < RACE_PROGRESS_DIGIT_COUNT && text[digit] == ' ')
    {
      text[digit] = '0';
      digit++;
    }
}


static
void
_format_race_numeric_readout(RaceNumericTextCache *cache_,
                            s32                   value_,
                            int                   zero_pad_)
{
  if((cache_->_text_bytes == 0) || (cache_->_value != value_))
    {
      cache_->_text_bytes = (size_t)(sprintf(hud_globals.formatted_text, "%3ld", value_) + 1);
      if(zero_pad_ != 0)
        {
          replace_leading_spaces_with_zeroes(hud_globals.formatted_text);
        }

      cache_->_value = value_;
      memcpy(cache_->_text, hud_globals.formatted_text, cache_->_text_bytes);
    }
  else
    {
      // Preserve the shared buffer's untouched tail, including after shorter text.
      memcpy(hud_globals.formatted_text, cache_->_text, cache_->_text_bytes);
    }
}


void
draw_player_progress_readout(void)
{
  DisplayRectangle background;
  HudCoordinate *position;
  SignedDivisionResult division;
  int progress;
  int layout;

  background.left = RACE_PROGRESS_BACKGROUND_LEFT;
  background.top = RACE_PROGRESS_BACKGROUND_TOP;
  background.right = RACE_PROGRESS_BACKGROUND_RIGHT;
  background.bottom = RACE_PROGRESS_BACKGROUND_BOTTOM;

  if(runtime_feature_flags.alternate_hud_layout.value != 0)
    {
      append_solid_rectangle_cel(
        &background, RACE_PROGRESS_BACKGROUND_COLOR);
    }

  progress = player_track_progress_units() - RACE_PROGRESS_ORIGIN;
  division = signed_divide_with_remainder(
    RACE_PROGRESS_UNIT_SPAN, (unsigned int)progress);
  _format_race_numeric_readout(&g_RACE_PROGRESS_TEXT_CACHE, division.quotient, 1);

  layout = runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals
             .coordinates[RACE_PROGRESS_COORDINATE_BASE + layout];
  draw_text(current_display_bitmap_item(),
            position->x, position->y,
            (int)hud_globals.formatted_text,
            HUD_TEXT_CHARACTER_SPACING,
            HUD_TEXT_ALIGNMENT_LEFT);
}


void
draw_player_engine_pitch_needle(void)
{
  RacerEntity *player;
  HudCoordinate *position;
  int target_pitch;
  int displayed_pitch;
  int pitch_delta;
  int layout;

  player = (RacerEntity *)race_rider_state.player_object;
  target_pitch = player->engine_pitch - ENGINE_PITCH_GAUGE_ORIGIN;
  displayed_pitch = hud_globals.smoothed_speed;
  pitch_delta = target_pitch - displayed_pitch;

  if(pitch_delta > ENGINE_PITCH_GAUGE_STEP)
    {
      displayed_pitch += ENGINE_PITCH_GAUGE_STEP;
    }
  else if(displayed_pitch - target_pitch <=
          ENGINE_PITCH_GAUGE_STEP)
    {
      displayed_pitch = target_pitch;
    }
  else
    {
      displayed_pitch -= ENGINE_PITCH_GAUGE_STEP;
    }

  if(displayed_pitch < ENGINE_PITCH_GAUGE_MINIMUM)
    {
      displayed_pitch = ENGINE_PITCH_GAUGE_MINIMUM;
    }
  hud_globals.smoothed_speed = displayed_pitch;

  layout = runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals
             .coordinates[ENGINE_PITCH_COORDINATE_BASE + layout];
  draw_speedometer_needle(
    displayed_pitch >> ENGINE_PITCH_GAUGE_ANGLE_SHIFT,
    position->x, position->y);
}


void
draw_player_race_position_readout(void)
{
  RacerEntity *player;
  RacerEntity *race_state;
  HudCoordinate *position;
  int race_position;
  int layout;
  int coordinate_base;
  int alignment;

  player = (RacerEntity *)race_rider_state.player_object;
  race_state = player->paired_state;
  if((race_state->status_flags & RACER_FINISHED_FLAG) != 0)
    {
      race_position = race_mode_state.finish_position + 1;
      race_rider_state.status =
        (int)(((unsigned int)race_rider_state.status &
               RACE_POSITION_STATUS_LOW_MASK) |
              ((unsigned int)(unsigned char)race_position <<
               RACE_POSITION_STATUS_SHIFT));
    }
  else
    {
      race_position = race_state->race_rank;
    }

  _format_race_numeric_readout(&g_RACE_POSITION_TEXT_CACHE, race_position, 0);

  layout = runtime_feature_flags.alternate_hud_layout.value;
  if(layout != 0)
    {
      coordinate_base = RACE_POSITION_ALTERNATE_COORDINATE_BASE;
      alignment = HUD_TEXT_ALIGNMENT_RIGHT;
    }
  else
    {
      coordinate_base = RACE_POSITION_STANDARD_COORDINATE_BASE;
      alignment = HUD_TEXT_ALIGNMENT_LEFT;
    }
  position = &hud_globals.coordinates[coordinate_base + layout];
  draw_text(current_display_bitmap_item(),
            position->x, position->y,
            (int)hud_globals.formatted_text,
            HUD_TEXT_CHARACTER_SPACING, alignment);
}

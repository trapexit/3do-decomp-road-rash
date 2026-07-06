#include "stdio.h"
/* Reconstructed from the original function at 0x00009ED4. */



#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"
#include "text_render_runtime.h"

int
submit_cel_batch(void);

enum RaceResultRowConstant
{
  RACE_RESULT_VISIBLE_ROW_COUNT = 4,
  RACE_RESULT_LAST_VISIBLE_ROW = 3,
  RACE_RESULT_TEXT_BUFFER_BYTES = 0x20,
  RACE_RESULT_NAME_X = 0x1F,
  RACE_RESULT_TIME_X = 0xA4,
  RACE_RESULT_CASH_X = 0xFB,
  RACE_RESULT_FIRST_ROW_Y = 0x55,
  RACE_RESULT_SECOND_ROW_Y = 0x65,
  RACE_RESULT_THIRD_ROW_Y = 0x74,
  RACE_RESULT_FOURTH_ROW_Y = 0x84,
  RACE_RESULT_SELECTED_RED = 0xF7,
  RACE_RESULT_SELECTED_GREEN = 0xD6,
  RACE_RESULT_SELECTED_BLUE = 0x10,
  RACE_RESULT_NORMAL_INTENSITY = 0xC6,
  RACE_RESULT_TICKS_PER_SECOND = 100,
  RACE_RESULT_SECONDS_PER_MINUTE = 60,
  RACE_RESULT_TICKS_PER_MINUTE = 0x1770,
  RACE_RESULT_LATEST_DISPLAYABLE_TICK = 0x55027,
  RACE_RESULT_RANK_ORIGIN = 1,
  RACE_RESULT_CASH_AWARD_SCALE = 10
};

typedef struct RaceResultRowLayout
{
  DisplayPoint rank_and_name;
  DisplayPoint elapsed_time;
  DisplayPoint cash_award;
} RaceResultRowLayout;

static const RaceResultRowLayout race_result_row_layouts[
  RACE_RESULT_VISIBLE_ROW_COUNT] =
{
  {
    { RACE_RESULT_NAME_X, RACE_RESULT_FIRST_ROW_Y },
    { RACE_RESULT_TIME_X, RACE_RESULT_FIRST_ROW_Y },
    { RACE_RESULT_CASH_X, RACE_RESULT_FIRST_ROW_Y }
  },
  {
    { RACE_RESULT_NAME_X, RACE_RESULT_SECOND_ROW_Y },
    { RACE_RESULT_TIME_X, RACE_RESULT_SECOND_ROW_Y },
    { RACE_RESULT_CASH_X, RACE_RESULT_SECOND_ROW_Y }
  },
  {
    { RACE_RESULT_NAME_X, RACE_RESULT_THIRD_ROW_Y },
    { RACE_RESULT_TIME_X, RACE_RESULT_THIRD_ROW_Y },
    { RACE_RESULT_CASH_X, RACE_RESULT_THIRD_ROW_Y }
  },
  {
    { RACE_RESULT_NAME_X, RACE_RESULT_FOURTH_ROW_Y },
    { RACE_RESULT_TIME_X, RACE_RESULT_FOURTH_ROW_Y },
    { RACE_RESULT_CASH_X, RACE_RESULT_FOURTH_ROW_Y }
  }
};

static const char race_result_rank_name_format[] = "%2ld %s";
static const char race_result_rank_player_format[] = "%2ld Player %d";
static const char race_result_slow_time_text[] = "DOG SLOW";
static const char race_result_time_format[] = "%2ld:%02ld.%02ld";
static const char race_result_cash_format[] = "$%5ld";

void
draw_race_result_row(int race_place)
{
  char text[RACE_RESULT_TEXT_BUFFER_BYTES];
  const RaceResultRowLayout *layout;
  int screen_item;
  int row_index;
  int rank;
  int rider_name_index;
  int elapsed_ticks;
  int player_rider_type;
  int cash;
  SignedDivisionResult division;
  SignedDivisionResult seconds_division;
  int minutes;
  int seconds;
  int hundredths;

  screen_item = display_runtime_globals.screen_items[0];

  row_index = race_place;
  if(row_index > RACE_RESULT_LAST_VISIBLE_ROW)
    {
      row_index = RACE_RESULT_LAST_VISIBLE_ROW;
    }
  layout = &race_result_row_layouts[row_index];

  if((int)race_mode_state.finish_position == race_place)
    {
      if(race_mode_state.game_mode == FRONT_END_GAME_MODE_ALTERNATE)
        {
          race_results[row_index].rider_name_index = RIDER_TYPE_PLAYER_ONE;
        }
      set_text_color(RACE_RESULT_SELECTED_RED,
                     RACE_RESULT_SELECTED_GREEN,
                     RACE_RESULT_SELECTED_BLUE);
    }
  else
    {
      set_text_color(RACE_RESULT_NORMAL_INTENSITY,
                     RACE_RESULT_NORMAL_INTENSITY,
                     RACE_RESULT_NORMAL_INTENSITY);
    }

  rider_name_index = race_results[row_index].rider_name_index;
  elapsed_ticks = race_results[row_index].elapsed_ticks;
  rank = race_place + RACE_RESULT_RANK_ORIGIN;

  if(rider_name_index > RIDER_TYPE_PLAYER_ONE)
    {
      sprintf(text, race_result_rank_name_format, rank,
              rider_names[rider_name_index]);
    }
  else if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      player_rider_type =
        player_profiles[race_mode_state.active_player_index].rider_type;
      sprintf(text, race_result_rank_name_format, rank,
              rider_names[player_rider_type]);
    }
  else
    {
      sprintf(text, race_result_rank_player_format, rank,
              (int)race_mode_state.active_player_index +
              RACE_RESULT_RANK_ORIGIN);
    }

  draw_text(screen_item,
            layout->rank_and_name.x, layout->rank_and_name.y,
            (int)text, 0, TEXT_ALIGNMENT_LEFT);

  if(elapsed_ticks <= RACE_RESULT_LATEST_DISPLAYABLE_TICK)
    {
      division = signed_divide_with_remainder(
        RACE_RESULT_TICKS_PER_MINUTE, elapsed_ticks);
      minutes = division.quotient;

      division = signed_divide_with_remainder(
        RACE_RESULT_TICKS_PER_SECOND, elapsed_ticks);
      hundredths = division.remainder;

      seconds_division = signed_divide_with_remainder(
        RACE_RESULT_SECONDS_PER_MINUTE, division.quotient);
      seconds = seconds_division.remainder;

      sprintf(text, race_result_time_format,
              minutes, seconds, hundredths);
    }
  else
    {
      sprintf(text, race_result_slow_time_text);
    }

  draw_text(screen_item,
            layout->elapsed_time.x, layout->elapsed_time.y,
            (int)text, 0, TEXT_ALIGNMENT_RIGHT);

  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      cash = ((int)player_profiles[race_mode_state.active_player_index]
              .level + 1) *
             race_cash_awards[race_place] * RACE_RESULT_CASH_AWARD_SCALE;
      sprintf(text, race_result_cash_format, cash);
      draw_text(screen_item,
                layout->cash_award.x, layout->cash_award.y,
                (int)text, 0, TEXT_ALIGNMENT_RIGHT);
    }

  submit_cel_batch();
}

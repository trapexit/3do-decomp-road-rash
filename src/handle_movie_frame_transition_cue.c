#include "cpak_player_runtime.h"
#include "display_system_runtime.h"
#include "graphics.h"

#define MOVIE_FADE_DURATION 0xF0
#define MOVIE_FADE_NUMERATOR_SCALE 60

typedef struct MovieFadeDivisionResult
{
  uint32 quotient;
  uint32 remainder;
} MovieFadeDivisionResult;

__value_in_regs MovieFadeDivisionResult
signed_divide_with_remainder(uint32 divisor,
                             uint32 numerator);
int
queue_fullscreen_clear(int32 screen_index);
void
present_display_frame(int32 screen_index);

void
handle_movie_frame_transition_cue(void *unused_context,
                                  int32 relative_stream_time)
{
  MovieFadeDivisionResult division;

  (void)unused_context;
  if(relative_stream_time < 0)
    {
      division = signed_divide_with_remainder(
        MOVIE_FADE_DURATION,
        (uint32)(-(relative_stream_time *
                   MOVIE_FADE_NUMERATOR_SCALE)));
      fade_display_screens_to_black((int32)division.quotient);
      queue_fullscreen_clear(0);
      present_display_frame(1);
      queue_fullscreen_clear(0);
      present_display_frame(1);
      ResetScreenColors(get_display_screen_item(0));
      ResetScreenColors(get_display_screen_item(1));
      return;
    }

  queue_fullscreen_clear(0);
  present_display_frame(1);
  queue_fullscreen_clear(0);
  present_display_frame(1);
}

#include "io.h"

#include "graphics.h"

#include "display_system_runtime.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"

enum DisplayFadeConstant
{
  DISPLAY_FADE_CLUT_COLOR_COUNT = 32,
  DISPLAY_FADE_MAXIMUM_INTENSITY = 0xFF
};

int
fade_display_screens_to_black(int frame_count)
{
  int remaining_frames;
  int fade_divisor;
  int color_index;
  int intensity_step;
  int intensity;
  uint32 color_entry;
  UnsignedDivisionResult unsigned_division;
  SignedDivisionResult signed_division;

  remaining_frames = frame_count - 1;
  fade_divisor = remaining_frames;
  if(remaining_frames >= 0)
    {
      do
        {
          if(WaitVBLDefer(
               display_system_globals.vbl_io_request, 1) >= 0)
            {
              WaitIO(display_system_globals.vbl_io_request);
            }
          unsigned_division = unsigned_divide_with_remainder(
            (unsigned int)fade_divisor,
            (unsigned int)(remaining_frames *
                           DISPLAY_FADE_MAXIMUM_INTENSITY));
          intensity_step = (int)unsigned_division.quotient;

          color_index = 0;
          do
            {
              signed_division = signed_divide_with_remainder(
                DISPLAY_FADE_CLUT_COLOR_COUNT - 1,
                (unsigned int)(color_index * intensity_step));
              intensity = signed_division.quotient &
                          DISPLAY_FADE_MAXIMUM_INTENSITY;
              color_entry = MakeCLUTColorEntry(
                color_index, intensity, intensity, intensity);

              SetScreenColor(
                display_system_globals.screen_items[0], color_entry);
              SetScreenColor(
                display_system_globals.screen_items[1], color_entry);

              color_index = (color_index + 1) &
                            DISPLAY_FADE_MAXIMUM_INTENSITY;
            } while(color_index < DISPLAY_FADE_CLUT_COLOR_COUNT);

          color_entry = MakeCLUTColorEntry(
            DISPLAY_FADE_CLUT_COLOR_COUNT, 0, 0, 0);
          SetScreenColor(
            display_system_globals.screen_items[0], color_entry);
          SetScreenColor(
            display_system_globals.screen_items[1], color_entry);
          remaining_frames--;
        } while(remaining_frames >= 0);
    }

  return SetScreenColor(
    0, MakeCLUTColorEntry(
      DISPLAY_FADE_CLUT_COLOR_COUNT, 0, 0, 0));
}

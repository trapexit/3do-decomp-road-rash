#include "display_cel_runtime.h"

CCB *
append_solid_quad_cel(const DisplayQuad *quad,
                      unsigned int       color)
{
  unsigned int *pixel_cursor;
  unsigned int pixel_word;
  CCB *cel;

  pixel_cursor = display_runtime_globals.solid_pixel_cursor;
  if(pixel_cursor > display_runtime_globals.solid_pixel_limit)
    {
      submit_cel_batch();
      pixel_cursor = display_runtime_globals.solid_pixel_cursor;
    }

  cel = append_unmapped_cel(display_runtime_globals.solid_fill_ccb);
  map_solid_quad_to_cel(cel, quad);

  pixel_word =
    color | (color << DISPLAY_COORDINATE_FRACTION_SHIFT);
  cel->ccb_SourcePtr = (CelData *)pixel_cursor;
  pixel_cursor[0] = pixel_word;
  pixel_cursor[DISPLAY_SOLID_PIXEL_REPEAT_INDEX] = pixel_word;
  display_runtime_globals.solid_pixel_cursor =
    pixel_cursor + DISPLAY_SOLID_PIXEL_WORD_COUNT;
  return cel;
}

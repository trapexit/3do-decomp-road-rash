#include "io.h"

#include "display_system_runtime.h"
#include "graphics.h"
#include "rw_semantic_data.h"

enum DisplayBitmapFillConstant
{
  DISPLAY_BITMAP_FILL_REPEAT_SHIFT = 16
};

void
fill_active_display_bitmap(unsigned int color)
{
  int index;

  index = display_system_globals.active_screen;
  if(index < 0)
    {
      return;
    }

  if(SetVRAMPagesDefer(
       display_system_globals.vram_io_request,
       display_system_globals.bitmaps[index]->bm_Buffer,
       color | (color << DISPLAY_BITMAP_FILL_REPEAT_SHIFT),
       display_system_globals.bitmap_storage_pages,
       -1) >= 0)
    {
      WaitIO(display_system_globals.vram_io_request);
    }
}

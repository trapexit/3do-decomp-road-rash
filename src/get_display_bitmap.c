
#include "graphics.h"
#include "rw_semantic_data.h"

enum DisplayBitmapLookupConstant
{
  DISPLAY_BITMAP_INDEX_MASK = 0xFF,
  DISPLAY_BITMAP_UNAVAILABLE = 0x10
};

int
get_display_bitmap(int screen_index)
{
  int index;

  screen_index &= DISPLAY_BITMAP_INDEX_MASK;
  if(display_system_globals.bitmap_items != 0)
    {
      if(screen_index < display_system_globals.screen_count)
        {
          return (int)display_system_globals.bitmaps[screen_index];
        }
      return 0;
    }

  index = display_system_globals.active_screen;
  if(index < 0)
    {
      return DISPLAY_BITMAP_UNAVAILABLE;
    }

  return DisplayScreen(display_system_globals.screen_items[index], 0);
}

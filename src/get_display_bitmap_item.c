#include "rw_semantic_data.h"

enum
{
  DISPLAY_BITMAP_INDEX_MASK = 0xFF
};

int
get_display_bitmap_item(int screen_index)
{
  Item *bitmap_items;

  screen_index &= DISPLAY_BITMAP_INDEX_MASK;
  bitmap_items = display_system_globals.bitmap_items;
  if(bitmap_items != 0 &&
     screen_index < display_system_globals.screen_count)
    {
      return bitmap_items[screen_index];
    }
  return 0;
}

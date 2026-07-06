#include "rw_semantic_data.h"

void
select_display_system_screen(int value);

enum { DISPLAY_BITMAP_SELECTOR_MASK = 0xFF };

void
select_display_bitmap(int buffer_index)
{
  if((buffer_index & DISPLAY_BITMAP_SELECTOR_MASK) == 0)
    {
      display_runtime_globals.active_bitmap_index =
        display_runtime_globals.draw_buffer_index;
    }
  else
    {
      display_runtime_globals.active_bitmap_index =
        display_runtime_globals.display_buffer_index;
    }
  select_display_system_screen(
    display_runtime_globals.active_bitmap_index);
}

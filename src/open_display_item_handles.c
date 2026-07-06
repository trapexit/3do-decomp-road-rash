#include "item.h"

#include "display_cel_runtime.h"

int
open_display_item_handles(void)
{
  Item opened[4];
  int index;
  int count;

  count = 0;
  for(index = 0; index < 4; index++)
    {
      opened[index] = OpenItem(
        index < 2 ? display_runtime_globals.saved_bitmap_items[index] :
                    display_runtime_globals.saved_screen_items[index - 2],
        0);
      if(opened[index] < 0)
        {
          while(count > 0)
            {
              CloseItem(opened[--count]);
            }
          return 0;
        }
      count++;
    }
  for(index = 0; index < 2; index++)
    {
      display_runtime_globals.bitmap_items[index] = opened[index];
      display_runtime_globals.screen_items[index] = opened[index + 2];
    }
  return 1;
}


void
close_display_item_handles(void)
{
  CloseItem(display_runtime_globals.bitmap_items[0]);
  display_runtime_globals.bitmap_items[0] =
    display_runtime_globals.saved_bitmap_items[0];
  CloseItem(display_runtime_globals.bitmap_items[1]);
  display_runtime_globals.bitmap_items[1] =
    display_runtime_globals.saved_bitmap_items[1];

  CloseItem(display_runtime_globals.screen_items[0]);
  display_runtime_globals.screen_items[0] =
    display_runtime_globals.saved_screen_items[0];
  CloseItem(display_runtime_globals.screen_items[1]);
  display_runtime_globals.screen_items[1] =
    display_runtime_globals.saved_screen_items[1];
}

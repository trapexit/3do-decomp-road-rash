#include "graphics.h"

#include "rw_semantic_data.h"

enum DisplayScreenAccessorConstant
{
  DISPLAY_SCREEN_ITEM_BYTE_SHIFT = 2,
  DISPLAY_CEL_DRAW_COMPLETION_CODE = 0x10
};

int
get_active_display_screen_index(void)
{
  return display_system_globals.active_screen;
}


int
get_active_display_screen_item(void)
{
  int index;

  index = display_system_globals.active_screen;
  if(index < 0)
    {
      return 0;
    }
  return display_system_globals.screen_items[index];
}


int
write_display_screen_items_to_destinations(int *destinations)
{
  int count;
  int index;
  int *destination;

  count = display_system_globals.screen_count;
  *destinations = count << DISPLAY_SCREEN_ITEM_BYTE_SHIFT;
  if(count == 0)
    {
      return 1;
    }

  index = 0;
  while(index < display_system_globals.screen_count)
    {
      destination = (int *)destinations[index];
      *destination = display_system_globals.screen_items[index];
      index++;
    }
  return display_system_globals.screen_count;
}


int
draw_cels_to_active_screen(CCB *ccb)
{
  int index;

  index = display_system_globals.active_screen;
  if(index >= 0)
    {
      DrawScreenCels(display_system_globals.screen_items[index], ccb);
    }
  return DISPLAY_CEL_DRAW_COMPLETION_CODE;
}

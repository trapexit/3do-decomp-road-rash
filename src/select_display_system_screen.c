#include "rw_semantic_data.h"

void
select_display_system_screen(int screen_index)
{
  unsigned int index;

  index = (unsigned char)screen_index;
  if(index < display_system_globals.screen_count)
    {
      display_system_globals.active_screen = (signed char)index;
    }
}

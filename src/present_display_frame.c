#include "io.h"

#include "display_cel_runtime.h"

void
present_display_frame(int wait_for_vbl)
{
  int synchronize;
  int screen_item;

  synchronize = (unsigned char)wait_for_vbl;
  submit_cel_batch();

  select_display_system_screen(display_runtime_globals.draw_buffer_index);

  screen_item = display_runtime_globals
                .screen_items[display_runtime_globals.draw_buffer_index];
  DisplayScreen(screen_item, screen_item);

  if(synchronize != 0 &&
     display_runtime_globals.synchronize_to_vbl != 0)
    {
      display_runtime_globals.last_present_vbl =
        (int)GrafBase->gf_VBLNumber;
    }

  display_runtime_globals.display_buffer_index =
    advance_display_buffer_index(
      display_runtime_globals.display_buffer_index);
  display_runtime_globals.draw_buffer_index =
    advance_display_buffer_index(display_runtime_globals.draw_buffer_index);
  display_runtime_globals.active_bitmap_index =
    advance_display_buffer_index(
      display_runtime_globals.active_bitmap_index);

  select_display_system_screen(display_runtime_globals.active_bitmap_index);

  if(synchronize == 0)
    {
      return;
    }

  if(display_runtime_globals.synchronize_to_vbl == 0)
    {
      if(WaitVBLDefer(display_runtime_globals.vbl_io_request, 1) >= 0)
        {
          WaitIO(display_runtime_globals.vbl_io_request);
        }
    }
}

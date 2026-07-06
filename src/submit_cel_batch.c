#include "display_cel_runtime.h"

int
submit_cel_batch(void)
{
  CCB *cursor;
  CCB *last_packet;
  volatile GrafFolio *device;
  int flush_target;

  cursor = display_runtime_globals.cel_packet_cursor;
  if(cursor == display_runtime_globals.cel_packet_base)
    {
      return (int)cursor;
    }

  last_packet = cursor - 1;
  last_packet->ccb_Flags |= CCB_LAST;

  if(display_runtime_globals.synchronize_to_vbl != 0)
    {
      flush_target = display_runtime_globals.last_present_vbl;
      device = (volatile GrafFolio *)GrafBase;
      while((unsigned int)flush_target == device->gf_VBLNumber)
        {
        }
    }

  DrawCels(display_runtime_globals
           .bitmap_items[display_runtime_globals.active_bitmap_index],
           display_runtime_globals.cel_packet_base);
  display_runtime_globals.cel_packet_cursor =
    display_runtime_globals.cel_packet_base;
  display_runtime_globals.solid_pixel_cursor =
    display_runtime_globals.solid_pixel_base;
  return (int)display_runtime_globals.solid_pixel_cursor;
}

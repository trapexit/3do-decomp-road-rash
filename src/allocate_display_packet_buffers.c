#include "graphics.h"
#include "memory_manager_runtime.h"
#include "limits.h"
#include "rw_semantic_data.h"

enum DisplayPacketAllocationConstant
{
  DISPLAY_CEL_PACKET_BYTES = 0x44,
  DISPLAY_SOLID_PIXEL_PACKET_BYTES = 0x10,
  DISPLAY_SOLID_PIXEL_WORDS = 4
};

int
allocate_display_packet_buffers(int packet_count)
{
  CCB *cel_packets;
  CCB *cel_cursor;
  CCB *next_cel;
  unsigned int *solid_pixels;
  int index;

  if(packet_count <= 0 || packet_count > INT_MAX / DISPLAY_CEL_PACKET_BYTES)
    {
      return 0;
    }
  if(display_runtime_globals.cel_packet_base != 0 ||
     display_runtime_globals.solid_pixel_base != 0)
    {
      return display_runtime_globals.cel_packet_base != 0 &&
        display_runtime_globals.solid_pixel_base != 0 &&
        display_runtime_globals.cel_packet_capacity == packet_count;
    }

  cel_packets = (CCB *)MEM_NewPointer(
    packet_count * DISPLAY_CEL_PACKET_BYTES,
    MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
  display_runtime_globals.cel_packet_base = cel_packets;
  if(cel_packets == 0)
    {
      goto dispose_solid_pixels;
    }

  display_runtime_globals.cel_packet_cursor = cel_packets;
  display_runtime_globals.cel_packet_capacity = packet_count;

  solid_pixels = (unsigned int *)MEM_NewPointer(
    packet_count * DISPLAY_SOLID_PIXEL_PACKET_BYTES,
    MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
  display_runtime_globals.solid_pixel_base = solid_pixels;
  if(solid_pixels == 0)
    {
      goto dispose_cel_packets;
    }

  display_runtime_globals.solid_pixel_cursor = solid_pixels;
  display_runtime_globals.solid_pixel_capacity = packet_count;
  display_runtime_globals.cel_packet_limit =
    cel_packets + packet_count - 1;
  display_runtime_globals.solid_pixel_limit =
    solid_pixels + packet_count * DISPLAY_SOLID_PIXEL_WORDS -
    DISPLAY_SOLID_PIXEL_WORDS;

  cel_cursor = cel_packets;
  index = 0;
  if(packet_count > 0)
    {
      do
        {
          next_cel = cel_cursor + 1;
          cel_cursor->ccb_NextPtr = next_cel;
          cel_cursor = next_cel;
          index++;
        } while(index < display_runtime_globals.cel_packet_capacity);
    }
  return 1;

dispose_cel_packets:
  cel_packets = display_runtime_globals.cel_packet_base;
  if(cel_packets != 0)
    {
      MEM_DisposePointer(cel_packets);
    }

dispose_solid_pixels:
  solid_pixels = display_runtime_globals.solid_pixel_base;
  if(solid_pixels != 0)
    {
      MEM_DisposePointer(solid_pixels);
    }
  display_runtime_globals.cel_packet_base = 0;
  display_runtime_globals.cel_packet_cursor = 0;
  display_runtime_globals.cel_packet_limit = 0;
  display_runtime_globals.cel_packet_capacity = 0;
  display_runtime_globals.solid_pixel_base = 0;
  display_runtime_globals.solid_pixel_cursor = 0;
  display_runtime_globals.solid_pixel_limit = 0;
  display_runtime_globals.solid_pixel_capacity = 0;
  return 0;
}

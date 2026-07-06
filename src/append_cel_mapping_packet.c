#include "display_cel_shading.h"

CCB *
append_cel_mapping_packet(const CCB *source)
{
  CCB *packet;

  if(source == 0)
    {
      source = display_runtime_globals.fallback_ccb;
    }

  packet = display_runtime_globals.cel_packet_cursor;
  if(packet > display_runtime_globals.cel_packet_limit)
    {
      submit_cel_batch();
      packet = display_runtime_globals.cel_packet_cursor;
    }

  copy_display_cel_mapping_fields(packet, source, 0);
  packet->ccb_PIXC = shade_display_cel_pixc(packet, source->ccb_PIXC);
  display_runtime_globals.cel_packet_cursor = packet + 1;
  return packet;
}

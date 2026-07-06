#include "display_cel_shading.h"

CCB *
append_axis_aligned_cel(const CCB                          *source,
                        const DisplayAxisAlignedCelMapping *mapping)
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

  copy_display_cel_source_fields(packet, source, 0);
  packet->ccb_PIXC = shade_display_cel_pixc(packet, source->ccb_PIXC);

  packet->ccb_XPos = mapping->x;
  packet->ccb_YPos = mapping->y;
  packet->ccb_HDX =
    mapping->horizontal_delta << DISPLAY_CEL_MAPPING_SCALE_SHIFT;
  packet->ccb_HDY = 0;
  packet->ccb_VDX = 0;
  packet->ccb_VDY = mapping->vertical_delta;
  packet->ccb_HDDX = 0;
  packet->ccb_HDDY = 0;

  display_runtime_globals.cel_packet_cursor = packet + 1;
  return packet;
}

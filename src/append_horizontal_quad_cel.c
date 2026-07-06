#include "display_cel_shading.h"

CCB *
append_horizontal_quad_cel(const CCB         *source,
                           const DisplayQuad *quad)
{
  CCB *packet;
  int width_shift;
  int height_shift;
  int combined_shift;
  int top_width_delta;
  int bottom_width_delta;
  int left_x;
  int left_y;
  int right_x;
  int bottom_x;
  int bottom_y;

  if(source == 0)
    {
      source = display_runtime_globals.fallback_ccb;
    }

  /* Read the cursor once: only a submitted batch can move it, and that path
     reloads it explicitly.  Reloading unconditionally cost one load (about
     2% of the call) on every append. */
  packet = display_runtime_globals.cel_packet_cursor;
  if(packet > display_runtime_globals.cel_packet_limit)
    {
      submit_cel_batch();
      packet = display_runtime_globals.cel_packet_cursor;
    }

  copy_display_cel_source_fields(packet, source, CCB_LDSIZE | CCB_LDPRS);
  width_shift = source->ccb_Width & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  height_shift = source->ccb_Height & DISPLAY_CEL_SOURCE_SHIFT_MASK;
  packet->ccb_PIXC = shade_display_cel_pixc(packet, source->ccb_PIXC);

  /* Corners are read into locals before the first packet store: stores through
     packet otherwise alias quad, forcing a reload of every corner on each use. */
  left_x = quad->top_left.x;
  left_y = quad->top_left.y;
  right_x = quad->top_right.x;
  bottom_x = quad->bottom_left.x;
  bottom_y = quad->bottom_left.y;

  packet->ccb_XPos = left_x;
  packet->ccb_YPos = left_y;

  top_width_delta = right_x - left_x;
  packet->ccb_HDX =
    (top_width_delta << DISPLAY_CEL_MAPPING_SCALE_SHIFT) >> width_shift;
  packet->ccb_HDY = 0;

  packet->ccb_VDX = (bottom_x - left_x) >> height_shift;
  packet->ccb_VDY = (bottom_y - left_y) >> height_shift;

  bottom_width_delta = quad->bottom_right.x - bottom_x;
  combined_shift = width_shift + height_shift;
  packet->ccb_HDDX =
    ((bottom_width_delta - top_width_delta) <<
      DISPLAY_CEL_MAPPING_SCALE_SHIFT) >> combined_shift;
  packet->ccb_HDDY = 0;

  display_runtime_globals.cel_packet_cursor = packet + 1;
  return packet;
}

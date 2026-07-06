#include "display_cel_shading.h"

__inline
static
int
scale_cel_mapping_delta(int value,
                        int source_shift)
{
  if(source_shift >= DISPLAY_CEL_MAPPING_SCALE_SHIFT)
    {
      return value >>
             (source_shift - DISPLAY_CEL_MAPPING_SCALE_SHIFT);
    }
  return value <<
         (DISPLAY_CEL_MAPPING_SCALE_SHIFT - source_shift);
}


CCB *
append_mapped_quad_cel(const CCB         *source,
                       const DisplayQuad *quad)
{
  CCB *packet;
  int width_shift;
  int height_shift;
  int combined_shift;
  int horizontal_delta;
  int vertical_delta;
  int left_x;
  int left_y;
  int right_x;
  int right_y;
  int bottom_x;
  int bottom_y;
  int corner_x;
  int corner_y;

  if(source == 0)
    {
      source = display_runtime_globals.fallback_ccb;
    }

  /* Read the cursor once; only a submitted batch can move it. */
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
  right_y = quad->top_right.y;
  bottom_x = quad->bottom_left.x;
  bottom_y = quad->bottom_left.y;
  corner_x = quad->bottom_right.x;
  corner_y = quad->bottom_right.y;

  packet->ccb_XPos = left_x;
  packet->ccb_YPos = left_y;

  horizontal_delta = left_x - right_x;
  vertical_delta = left_y - right_y;
  packet->ccb_HDX =
    scale_cel_mapping_delta(-horizontal_delta, width_shift);
  packet->ccb_HDY =
    scale_cel_mapping_delta(-vertical_delta, width_shift);

  packet->ccb_VDX = (bottom_x - left_x) >> height_shift;
  packet->ccb_VDY = (bottom_y - left_y) >> height_shift;

  combined_shift = width_shift + height_shift;
  horizontal_delta += corner_x - bottom_x;
  vertical_delta += corner_y - bottom_y;
  packet->ccb_HDDX =
    scale_cel_mapping_delta(horizontal_delta, combined_shift);
  packet->ccb_HDDY =
    scale_cel_mapping_delta(vertical_delta, combined_shift);

  display_runtime_globals.cel_packet_cursor = packet + 1;
  return packet;
}

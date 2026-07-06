#include "io.h"

#include "graphics.h"
/* Reconstructed from the original function at 0x00007A00. */



#include "rw_semantic_data.h"

#include "display_system_runtime.h"
#include "division_helpers.h"
#include "item.h"

enum
{
  DISPLAY_BITMAP_BYTES_PER_PIXEL = 2
};

int
copy_display_bitmap(int destination_index,
                    int source_index)
{
  int src_index;
  int dst_index;
  Item vram_io_item;
  Bitmap *dimensions;
  Bitmap *source;
  Bitmap *destination;
  int covered_bytes;
  int page_count;

  dst_index = (signed char)destination_index;
  src_index = (signed char)source_index;

  vram_io_item = GetVRAMIOReq();
  if(src_index == dst_index)
    {
      return vram_io_item;
    }

  if(WaitVBLDefer(display_runtime_globals.vbl_io_request, 1) >= 0)
    {
      WaitIO(display_runtime_globals.vbl_io_request);
    }

  dimensions = display_runtime_globals.bitmaps[0];
  covered_bytes = GrafBase->gf_VRAMPageSize;
  covered_bytes += dimensions->bm_Height * dimensions->bm_Width *
                   DISPLAY_BITMAP_BYTES_PER_PIXEL;
  page_count = unsigned_divide_with_remainder(
    GrafBase->gf_VRAMPageSize,
    (unsigned int)(covered_bytes - 1)).quotient;

  source = display_runtime_globals.bitmaps[src_index];
  destination = display_runtime_globals.bitmaps[dst_index];
  if(CopyVRAMPagesDefer(
       vram_io_item, destination->bm_Buffer, source->bm_Buffer,
       (uint32)page_count, (uint32) - 1) >= 0)
    {
      WaitIO(vram_io_item);
    }

  return DeleteItem(vram_io_item);
}

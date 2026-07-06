#include "cel_dimension_resource.h"
#include "hardware.h"
#include "rw_semantic_data.h"

enum CelFirstPixelValue
{
  CEL_PACK_CONTROL_MASK = 0x00C00000,
  CEL_PACK_CONTROL_SHIFT = 22,
  CEL_PACK_PIXEL_MASK = 0x0000FF00,
  CEL_PACK_PIXEL_SHIFT = 8,
  CEL_PALETTE_INDEX_MASK = 0x1F,
  CEL_PALETTE_WORD_SHIFT = 1,
  CEL_PALETTE_HIGH_ENTRY_SHIFT = 16,
  CEL_PALETTE_COLOR_SHIFT = 17
};

int
is_cel_first_pixel_transparent(CCB *cel)
{
  unsigned int flags;
  unsigned int source_word;
  unsigned int pixel_index;
  unsigned int palette_word;
  unsigned int bits_per_pixel;

  if(cel == 0)
    {
      return 1;
    }

  bits_per_pixel = cel->ccb_PRE0 & PRE0_BPP_MASK;
  flags = cel->ccb_Flags;
  if((flags & CCB_PACKED) != 0)
    {
      source_word = *(unsigned int *)cel->ccb_SourcePtr;
      pixel_index =
        (source_word & CEL_PACK_CONTROL_MASK) >> CEL_PACK_CONTROL_SHIFT;
      if(pixel_index == PACK_EOL || pixel_index == PACK_TRANSPARENT)
        {
          return 1;
        }
      if((flags & CCB_BGND) != 0)
        {
          return 0;
        }
      pixel_index =
        (source_word & CEL_PACK_PIXEL_MASK) >> CEL_PACK_PIXEL_SHIFT;
    }
  else
    {
      if((flags & CCB_BGND) != 0)
        {
          return 0;
        }
      pixel_index =
        (unsigned int)*(unsigned char *)cel->ccb_SourcePtr >>
        CEL_PACK_CONTROL_SHIFT;
    }

  pixel_index >>= pixel_bit_shifts[bits_per_pixel];
  pixel_index &= CEL_PALETTE_INDEX_MASK;
  palette_word =
    ((unsigned int *)cel->ccb_PLUTPtr)[
      pixel_index >> CEL_PALETTE_WORD_SHIFT];
  if((pixel_index & 1) == 0)
    {
      palette_word >>= CEL_PALETTE_HIGH_ENTRY_SHIFT;
    }

  return ((palette_word << CEL_PALETTE_COLOR_SHIFT) == 0);
}

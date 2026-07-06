
#include "celutils.h"
#include "hardware.h"

#include "rw_semantic_data.h"

enum CelSourceClipConstant
{
  CEL_PREAMBLE_WORD_COUNT = 2,
  CEL_SIX_BPP_PIXEL_GROUP_SHIFT = 4,
  CEL_SIX_BPP_WORDS_PER_GROUP = 3
};

int
clip_cel_source_rectangle(const CCB   *source,
                          int          left_skip,
                          int          top_skip,
                          unsigned int width,
                          unsigned int height,
                          CCB         *destination)
{
  unsigned int flags;
  unsigned int preamble_zero;
  unsigned int preamble_one;
  unsigned int pixel_format;
  unsigned int row_word_count;
  unsigned int source_skip_pixels;
  const unsigned int *source_cursor;
  const unsigned int *source_data;

  flags = source->ccb_Flags;
  if((flags & CCB_PACKED) != 0)
    {
      return -1;
    }

  source_cursor = (const unsigned int *)CEL_DATAPTR(source);
  if((flags & CCB_CCBPRE) != 0)
    {
      preamble_zero = source->ccb_PRE0;
      preamble_one = source->ccb_PRE1;
    }
  else
    {
      preamble_zero = *source_cursor++;
      preamble_one = *source_cursor++;
    }

  pixel_format = preamble_zero & PRE0_BPP_MASK;
  row_word_count = CEL_PREAMBLE_WORD_COUNT;
  if(pixel_format < PRE0_BPP_8)
    {
      row_word_count +=
        (preamble_one & PRE1_WOFFSET8_MASK) >>
        PRE1_WOFFSET8_SHIFT;
    }
  else
    {
      row_word_count +=
        (preamble_one & PRE1_WOFFSET10_MASK) >>
        PRE1_WOFFSET10_SHIFT;
    }

  source_data = source_cursor +
                (unsigned int)top_skip * row_word_count;
  source_skip_pixels = (unsigned int)left_skip +
                       ((preamble_zero & PRE0_SKIPX_MASK) >>
                        PRE0_SKIPX_SHIFT);

  if(pixel_format == PRE0_BPP_6)
    {
      source_data +=
        (source_skip_pixels >> CEL_SIX_BPP_PIXEL_GROUP_SHIFT) *
        CEL_SIX_BPP_WORDS_PER_GROUP;
      source_skip_pixels &= PRE0_SKIPX_MASK >> PRE0_SKIPX_SHIFT;
    }
  else
    {
      source_data +=
        source_skip_pixels >> cel_pixel_shifts[pixel_format];
      source_skip_pixels &= cel_pixel_masks[pixel_format];
    }

  destination->ccb_SourcePtr = (CelData *)source_data;
  destination->ccb_PLUTPtr = CEL_PLUTPTR(source);
  destination->ccb_Flags =
    flags | CCB_SPABS | CCB_PPABS | CCB_CCBPRE;
  destination->ccb_PIXC = source->ccb_PIXC;
  destination->ccb_PRE0 =
    (source_skip_pixels << PRE0_SKIPX_SHIFT) |
    ((height - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
    (preamble_zero & ~(PRE0_SKIPX_MASK | PRE0_VCNT_MASK));
  destination->ccb_PRE1 =
    (width + source_skip_pixels - 1) |
    (preamble_one & ~PRE1_TLHPCNT_MASK);
  destination->ccb_Width = (int)width;
  destination->ccb_Height = (int)height;

  return 0;
}

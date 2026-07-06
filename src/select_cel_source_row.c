#include "display_cel_runtime.h"

enum CelSourceRowConstant
{
  PACKED_CEL_SOURCE_PREAMBLE_WORD_COUNT = 1,
  UNPACKED_CEL_SOURCE_PREAMBLE_WORD_COUNT = 2,
  PACKED_CEL_ROW_CONTROL_WORD_COUNT = 2,
  PACKED_CEL_ROW_WORD_COUNT_SHIFT = 16
};

CelData *
select_cel_source_row(CCB *cel,
                      int  source_row)
{
  uint32 pre0_word;
  uint32 pre1_word;
  unsigned int source_height;
  uint32 flags;
  unsigned int row_word_count;
  uint32 *source_words;
  int preambles_in_ccb;

  if(cel == 0)
    {
      return 0;
    }

  source_words = (uint32 *)cel->ccb_SourcePtr;
  if(source_row == 0)
    {
      return (CelData *)source_words;
    }

  read_cel_preamble_words(cel, &pre0_word, &pre1_word);

  source_height = PRE0_VCNT_PREFETCH +
                  ((pre0_word & PRE0_VCNT_MASK) >> PRE0_VCNT_SHIFT);
  if((unsigned int)source_row >= source_height)
    {
      source_row = source_height - PRE0_VCNT_PREFETCH;
    }

  cel->ccb_PRE0 = pre0_word;
  cel->ccb_PRE1 = pre1_word;

  flags = cel->ccb_Flags;
  preambles_in_ccb = flags & CCB_CCBPRE;
  if((flags & CCB_PACKED) != 0)
    {
      if(preambles_in_ccb == 0)
        {
          source_words += PACKED_CEL_SOURCE_PREAMBLE_WORD_COUNT;
        }

      if((pre0_word & PRE0_BPP_MASK) >= PRE0_BPP_8)
        {
          while(source_row > 0)
            {
              row_word_count =
                source_words[0] >> PACKED_CEL_ROW_WORD_COUNT_SHIFT;
              source_words += row_word_count +
                              PACKED_CEL_ROW_CONTROL_WORD_COUNT;
              source_row--;
            }
        }
      else
        {
          while(source_row > 0)
            {
              row_word_count = *(unsigned char *)source_words;
              source_words += row_word_count +
                              PACKED_CEL_ROW_CONTROL_WORD_COUNT;
              source_row--;
            }
        }
    }
  else
    {
      if(preambles_in_ccb == 0)
        {
          source_words += UNPACKED_CEL_SOURCE_PREAMBLE_WORD_COUNT;
        }

      row_word_count = PRE1_WOFFSET_PREFETCH;
      if((pre0_word & PRE0_BPP_MASK) < PRE0_BPP_8)
        {
          row_word_count +=
            (pre1_word & PRE1_WOFFSET8_MASK) >> PRE1_WOFFSET8_SHIFT;
        }
      else
        {
          row_word_count +=
            (pre1_word & PRE1_WOFFSET10_MASK) >> PRE1_WOFFSET10_SHIFT;
        }
      source_words += row_word_count * source_row;
    }

  cel->ccb_Flags = flags | CCB_CCBPRE;
  return (CelData *)source_words;
}

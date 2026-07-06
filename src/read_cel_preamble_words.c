#include "display_cel_runtime.h"

uint32
read_cel_preamble_words(const CCB *cel,
                        uint32    *pre0_word,
                        uint32    *pre1_word)
{
  const uint32 *source_words;
  uint32 value;

  if((cel->ccb_Flags & CCB_CCBPRE) != 0)
    {
      *pre0_word = cel->ccb_PRE0;
      value = cel->ccb_PRE1;
      *pre1_word = value;
      return value;
    }

  source_words = (const uint32 *)cel->ccb_SourcePtr;
  *pre0_word = source_words[0];
  value = source_words[1];
  *pre1_word = value;
  return value;
}

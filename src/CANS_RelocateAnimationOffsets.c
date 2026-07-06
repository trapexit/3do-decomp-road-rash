#include "front_end_cel_runtime.h"

int
CANS_RelocateAnimationOffsets(CansOffsetAnimation *animation)
{
  uint32 *offset_word;

  offset_word = (uint32 *)&animation->frame_chunks;
  while((*offset_word & CANS_CHUNK_TAG_PREFIX_MASK) == 0)
    {
      *offset_word += (uint32)animation;
      offset_word++;
    }
  return 0;
}

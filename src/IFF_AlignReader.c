#include "iff_runtime.h"

int
IFF_AlignReader(ByteFileReader *reader)
{
  if((reader->position & IFF_CHUNK_ALIGNMENT_MASK) != 0)
    {
      reader->position++;
    }
  return 0;
}

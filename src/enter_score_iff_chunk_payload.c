#include "iff_runtime.h"
#include "score_program_runtime.h"

int
enter_score_iff_chunk_payload(ByteFileReader *reader)
{
  int result;

  result = BF_Seek(reader, IFF_CHUNK_HEADER_SIZE,
                   BYTE_FILE_SEEK_CURRENT);
  if(result >= 0)
    {
      result = 0;
    }
  return result;
}

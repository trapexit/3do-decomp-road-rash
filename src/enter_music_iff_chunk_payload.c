#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
enter_music_iff_chunk_payload(ByteFileReader *reader)
{
  int result;

  result = BF_Seek(reader, IFF_CHUNK_HEADER_SIZE,
                   BYTE_FILE_SEEK_CURRENT);
  if(result >= 0)
    {
      return 0;
    }
  music_library_globals.last_error = result;
  return result;
}

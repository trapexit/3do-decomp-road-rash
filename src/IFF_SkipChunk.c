#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
IFF_SkipChunk(ByteFileReader       *reader,
              const IffChunkHeader *header)
{
  int result;

  result = BF_Seek(reader, header->chunk_size + IFF_CHUNK_HEADER_SIZE,
                   BYTE_FILE_SEEK_CURRENT);
  if(result < 0)
    {
      block_file_api_globals.last_error = result;
      return result;
    }

  return 0;
}

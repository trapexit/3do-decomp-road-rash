#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
IFF_PeekChunkHeader(ByteFileReader *reader,
                    IffChunkHeader *header)
{
  int result;

  result = BF_Read(reader, header, IFF_CHUNK_HEADER_SIZE);
  if(result != IFF_CHUNK_HEADER_SIZE)
    {
      block_file_api_globals.last_error = -1;
      return -1;
    }

  result = BF_Seek(reader, -IFF_CHUNK_HEADER_SIZE,
                   BYTE_FILE_SEEK_CURRENT);
  if(result >= 0)
    {
      return 0;
    }

  block_file_api_globals.last_error = result;
  return result;
}

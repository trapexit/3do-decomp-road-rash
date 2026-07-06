#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
IFF_FindChunk(ByteFileReader *reader,
              unsigned int    chunk_id,
              int             start_position,
              IffChunkHeader *header)
{
  int result;
  unsigned int current_chunk_id;

  if(start_position >= 0)
    {
      result = BF_Seek(reader, start_position, BYTE_FILE_SEEK_SET);
      if(result < 0)
        {
          block_file_api_globals.last_error = result;
          return result;
        }
    }

  result = IFF_PeekChunkHeader(reader, header);
  while(result == 0)
    {
      current_chunk_id = 0;
      if(chunk_id != 0)
        {
          current_chunk_id = header->chunk_id;
        }
      if(chunk_id == 0 || current_chunk_id == chunk_id)
        {
          return 0;
        }

      result = IFF_SkipChunk(reader, header);
      if(result != 0)
        {
          break;
        }

      result = IFF_PeekChunkHeader(reader, header);
    }

  return block_file_api_globals.last_error;
}

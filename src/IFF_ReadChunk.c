#include "limits.h"
#include "iff_runtime.h"
#include "rw_semantic_data.h"

enum IffReadChunkError
{
  IFF_READ_CHUNK_ALLOCATION_ERROR = 0x1404
};

int
IFF_ReadChunk(ByteFileReader *reader,
              unsigned int    chunk_id,
              int             start_position,
              int             destination,
              int            *chunk_allocation_bytes)
{
  IffChunkHeader header;
  int allocation_bytes;
  int chunk_data;
  int result;

  if(IFF_FindChunk(reader, chunk_id, start_position, &header) != 0)
    {
      return 0;
    }

  if(header.chunk_size < 0 ||
     header.chunk_size > INT_MAX - IFF_CHUNK_HEADER_SIZE)
    {
      block_file_api_globals.last_error = -1;
      return 0;
    }
  allocation_bytes = header.chunk_size + IFF_CHUNK_HEADER_SIZE;
  chunk_data = destination;

  if(chunk_data == 0)
    {
      chunk_data = block_file_api_globals.allocate(allocation_bytes, 0);
      if(chunk_data == 0)
        {
          block_file_api_globals.last_error =
            IFF_READ_CHUNK_ALLOCATION_ERROR;
          return 0;
        }
    }

  if(start_position >= 0)
    {
      result = BF_Seek(reader, start_position, BYTE_FILE_SEEK_SET);
      if(result < 0)
        {
          block_file_api_globals.last_error = result;
          if(destination == 0)
            {
              block_file_api_globals.release(chunk_data, allocation_bytes);
            }
          return 0;
        }
    }

  result = BF_Read(reader, (void *)chunk_data, allocation_bytes);
  if(result != allocation_bytes)
    {
      block_file_api_globals.last_error = result < 0 ? result : -1;
      if(destination == 0)
        {
          block_file_api_globals.release(chunk_data, allocation_bytes);
        }
      return 0;
    }

  if(chunk_allocation_bytes != 0)
    {
      *chunk_allocation_bytes = allocation_bytes;
    }

  return chunk_data;
}

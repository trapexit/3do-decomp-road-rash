#include "io.h"

#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032D50. */
int
wait_for_next_async_block_stream_buffer(AsyncBlockStream *stream)
{
  int ready_index;
  int pending_index;
  int result;

  result = service_async_block_stream_reads(stream);
  if(result < 0)
    {
      return ASYNC_BLOCK_STREAM_NO_BUFFER;
    }
  select_oldest_async_block_stream_buffers(
    stream, &ready_index, &pending_index);

  if(ready_index == ASYNC_BLOCK_STREAM_NO_BUFFER)
    {
      if(pending_index == ASYNC_BLOCK_STREAM_NO_BUFFER)
        {
          if((stream->file_status.fs_ByteCount >>
              ASYNC_BLOCK_STREAM_BLOCK_SHIFT) >
             (unsigned int)stream->next_block)
            {
              return ASYNC_BLOCK_STREAM_MORE_DATA_PENDING;
            }
          return ASYNC_BLOCK_STREAM_NO_BUFFER;
        }

      result = WaitIO(stream->buffers[pending_index].io_request);
      if(result < 0)
        {
          stream->last_error = result;
          return ASYNC_BLOCK_STREAM_NO_BUFFER;
        }
      result = service_async_block_stream_reads(stream);
      if(result < 0)
        {
          return ASYNC_BLOCK_STREAM_NO_BUFFER;
        }
      select_oldest_async_block_stream_buffers(
        stream, &ready_index, &pending_index);
    }

  return ready_index;
}

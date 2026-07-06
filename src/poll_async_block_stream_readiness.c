#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032E68. */
int
poll_async_block_stream_readiness(AsyncBlockStream *stream)
{
  int buffer_index;
  int pending_buffer_seen;
  int buffer_count;
  AsyncStreamBuffer *buffer;

  if(stream == 0)
    {
      return -1;
    }

  service_async_block_stream_reads(stream);

  pending_buffer_seen = 0;
  buffer_index = 0;
  buffer_count = stream->buffer_count;
  if(buffer_count > 0)
    {
      do
        {
          buffer = &stream->buffers[buffer_index];
          if(buffer->state == ASYNC_STREAM_BUFFER_READY)
            {
              return 1;
            }
          if(buffer->state == ASYNC_STREAM_BUFFER_READ_PENDING)
            {
              pending_buffer_seen = 1;
            }
          buffer_index++;
        } while(buffer_index < buffer_count);
    }

  if(pending_buffer_seen != 0)
    {
      return 0;
    }

  if((stream->file_status.fs_ByteCount >>
      ASYNC_BLOCK_STREAM_BLOCK_SHIFT) >
     (unsigned int)stream->next_block)
    {
      return ASYNC_BLOCK_STREAM_MORE_DATA_PENDING;
    }
  return -1;
}

#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032F88. */
int
release_async_block_stream_chunk(AsyncBlockStream *stream,
                                 const void       *chunk)
{
  int buffer_index;
  int buffer_count;
  AsyncStreamBuffer *buffer;

  if(stream == 0 || chunk == 0)
    {
      return -1;
    }

  buffer_index = 0;
  buffer_count = stream->buffer_count;
  if(buffer_count > 0)
    {
      do
        {
          buffer = &stream->buffers[buffer_index];
          if(buffer->data == chunk)
            {
              if(buffer->state != ASYNC_STREAM_BUFFER_ACQUIRED)
                {
                  return -1;
                }
              buffer->state = ASYNC_STREAM_BUFFER_FREE;
              return queue_async_block_stream_reads(stream, 0);
            }
          buffer_index++;
        } while(buffer_index < buffer_count);
    }

  return -1;
}

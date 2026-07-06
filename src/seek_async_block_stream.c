#include "io.h"

#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032990. */
int
seek_async_block_stream(AsyncBlockStream *stream,
                        int               block_number,
                        int               wait_for_buffer)
{
  int buffer_index;
  int block_offset;
  AsyncStreamBuffer *buffer;

  wait_for_buffer = (unsigned char)wait_for_buffer;
  if(stream == 0 || block_number < 0)
    {
      return -1;
    }

  block_offset = (int)((unsigned int)block_number *
                       (unsigned int)stream->blocks_per_buffer);
  if((stream->file_status.fs_ByteCount >>
      ASYNC_BLOCK_STREAM_BLOCK_SHIFT) <=
     (unsigned int)block_offset)
    {
      return -1;
    }

  for(buffer_index = 0; buffer_index < stream->buffer_count;
      buffer_index++)
    {
      if(stream->buffers[buffer_index].state == ASYNC_STREAM_BUFFER_ACQUIRED)
        {
          return -1;
        }
    }

  buffer_index = 0;
  while(buffer_index < stream->buffer_count)
    {
      buffer = &stream->buffers[buffer_index];
      if(buffer->state == ASYNC_STREAM_BUFFER_READ_PENDING &&
         CheckIO(buffer->io_request) == 0)
        {
          AbortIO(buffer->io_request);
          WaitIO(buffer->io_request);
        }
      buffer->state = ASYNC_STREAM_BUFFER_FREE;
      buffer_index++;
    }

  stream->next_block = block_offset;
  stream->last_error = 0;
  return queue_async_block_stream_reads(stream, wait_for_buffer);
}

#include "string.h"

#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032DDC. */
int
read_async_block_stream_buffer(AsyncBlockStream *stream,
                               void             *destination)
{
  int buffer_index;
  int transfer_byte_count;
  AsyncStreamBuffer *buffer;

  if(stream == 0 || destination == 0)
    {
      return -1;
    }

  buffer_index = wait_for_next_async_block_stream_buffer(stream);
  if(buffer_index == ASYNC_BLOCK_STREAM_NO_BUFFER ||
     stream->last_error < 0)
    {
      return -1;
    }
  if(buffer_index == ASYNC_BLOCK_STREAM_MORE_DATA_PENDING)
    {
      return 0;
    }

  transfer_byte_count =
    stream->blocks_per_buffer << ASYNC_BLOCK_STREAM_BLOCK_SHIFT;
  buffer = &stream->buffers[buffer_index];
  memcpy(destination, buffer->data, transfer_byte_count);
  buffer->state = ASYNC_STREAM_BUFFER_FREE;
  queue_async_block_stream_reads(stream, 0);

  return transfer_byte_count;
}

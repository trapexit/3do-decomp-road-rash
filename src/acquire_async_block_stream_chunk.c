#include "rw_semantic_data.h"

#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032EF8. */
unsigned char *
acquire_async_block_stream_chunk(AsyncBlockStream *stream)
{
  int buffer_index;
  AsyncStreamBuffer *buffer;

  if(stream == 0)
    {
      block_file_last_error = ASYNC_BLOCK_STREAM_ERROR_BAD_POINTER;
      return 0;
    }

  buffer_index = wait_for_next_async_block_stream_buffer(stream);
  if(stream->last_error < 0)
    {
      block_file_last_error = stream->last_error;
      return 0;
    }
  if(buffer_index == ASYNC_BLOCK_STREAM_NO_BUFFER)
    {
      stream->last_error = ASYNC_BLOCK_STREAM_STATUS_END_OF_STREAM;
      block_file_last_error = ASYNC_BLOCK_STREAM_STATUS_END_OF_STREAM;
      return 0;
    }
  if(buffer_index == ASYNC_BLOCK_STREAM_MORE_DATA_PENDING)
    {
      stream->last_error = ASYNC_BLOCK_STREAM_STATUS_BUFFERING;
      block_file_last_error = ASYNC_BLOCK_STREAM_STATUS_BUFFERING;
      return 0;
    }

  buffer = &stream->buffers[buffer_index];
  buffer->state = ASYNC_STREAM_BUFFER_ACQUIRED;
  return buffer->data;
}

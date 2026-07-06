#include "io.h"

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

static
int
bytes_to_blocks_towards_zero(int byte_count)
{
  if(byte_count < 0)
    {
      byte_count += (1 << ASYNC_BLOCK_STREAM_BLOCK_SHIFT) - 1;
    }
  return byte_count >> ASYNC_BLOCK_STREAM_BLOCK_SHIFT;
}


/* Reconstructed from the original function at 0x00032BE8. */
int
service_async_block_stream_reads(AsyncBlockStream *stream)
{
  int buffer_index;
  int free_buffer_seen;
  int buffer_count;
  int completed_blocks;
  AsyncStreamBuffer *buffer;

  free_buffer_seen = 0;
  buffer_index = 0;
  buffer_count = stream->buffer_count;
  if(buffer_count <= 0)
    {
      return buffer_count;
    }

  do
    {
      buffer = &stream->buffers[buffer_index];
      if(buffer->state == ASYNC_STREAM_BUFFER_READ_PENDING)
        {
          if(CheckIO(buffer->io_request) != 0)
            {
              buffer = &stream->buffers[buffer_index];
              if(buffer->io_request_record->io_Error < 0)
                {
                  stream->last_error =
                    buffer->io_request_record->io_Error;
                  block_file_last_error = stream->last_error;
                  buffer->state = ASYNC_STREAM_BUFFER_SHORT_READ;
                  return stream->last_error;
                }
              completed_blocks = bytes_to_blocks_towards_zero(
                buffer->io_request_record->io_Actual);
              if(completed_blocks == stream->blocks_per_buffer)
                {
                  buffer->state = ASYNC_STREAM_BUFFER_READY;
                }
              else
                {
                  buffer->state = ASYNC_STREAM_BUFFER_SHORT_READ;
                }
            }
        }
      else if(buffer->state == ASYNC_STREAM_BUFFER_FREE)
        {
          free_buffer_seen = 1;
        }

      buffer_index++;
      buffer_count = stream->buffer_count;
    } while(buffer_index < buffer_count);

  if(free_buffer_seen != 0)
    {
      return queue_async_block_stream_reads(stream, 0);
    }
  return buffer_count;
}

#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00032CA8. */
void
select_oldest_async_block_stream_buffers(AsyncBlockStream *stream,
                                         int              *ready_index,
                                         int              *pending_index)
{
  int buffer_index;
  int buffer_count;
  int selected_index;
  AsyncStreamBuffer *buffer;

  *ready_index = -1;
  *pending_index = -1;

  buffer_index = 0;
  buffer_count = stream->buffer_count;
  if(buffer_count <= 0)
    {
      return;
    }

  do
    {
      buffer = &stream->buffers[buffer_index];
      if(buffer->state == ASYNC_STREAM_BUFFER_READY)
        {
          selected_index = *ready_index;
          if(selected_index == -1 ||
             buffer->block_offset <
             stream->buffers[selected_index].block_offset)
            {
              *ready_index = buffer_index;
            }
        }
      else if(buffer->state ==
              ASYNC_STREAM_BUFFER_READ_PENDING)
        {
          selected_index = *pending_index;
          if(selected_index == -1 ||
             buffer->block_offset <
             stream->buffers[selected_index].block_offset)
            {
              *pending_index = buffer_index;
            }
        }

      buffer_index++;
      buffer_count = stream->buffer_count;
    } while(buffer_index < buffer_count);
}

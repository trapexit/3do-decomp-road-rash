#include "io.h"
#include "string.h"

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

/* Reconstructed from the original function at 0x00032A68. */
int
queue_async_block_stream_reads(AsyncBlockStream *stream,
                               int               wait_for_buffer)
{
  IOInfo request;
  int buffer_index;
  int selected_index;
  int buffer_count;
  AsyncStreamBuffer *buffer;
  AsyncStreamBuffer *selected_buffer;
  int result;

  wait_for_buffer = (unsigned char)wait_for_buffer;
  if(stream == 0)
    {
      return -1;
    }
  if(stream->buffering_enabled == 0)
    {
      return 0;
    }

  selected_index = -1;
  buffer_count = stream->buffer_count;
  if(buffer_count > 0)
    {
      for(buffer_index = 0;
          buffer_index < buffer_count;
          buffer_index++)
        {
          buffer = &stream->buffers[buffer_index];
          if(buffer->state == ASYNC_STREAM_BUFFER_FREE)
            {
              if((stream->file_status.fs_ByteCount >>
                  ASYNC_BLOCK_STREAM_BLOCK_SHIFT) <=
                 (unsigned int)stream->next_block)
                {
                  break;
                }

              memset(&request, 0, sizeof(request));
              request.ioi_Command = CMD_READ;
              request.ioi_Recv.iob_Buffer = buffer->data;
              request.ioi_Recv.iob_Len =
                stream->blocks_per_buffer <<
                  ASYNC_BLOCK_STREAM_BLOCK_SHIFT;
              request.ioi_Offset = stream->next_block;

              result = SendIO(buffer->io_request, &request);
              if(result < 0)
                {
                  stream->last_error = result;
                  block_file_last_error = stream->last_error;
                  return stream->last_error;
                }

              buffer = &stream->buffers[buffer_index];
              buffer->block_offset = stream->next_block;
              stream->next_block += stream->blocks_per_buffer;
              buffer->state = ASYNC_STREAM_BUFFER_READ_PENDING;
              selected_index = buffer_index;
            }
          else if(buffer->state ==
                  ASYNC_STREAM_BUFFER_READ_PENDING)
            {
              if(selected_index == -1)
                {
                  selected_index = buffer_index;
                }
              else
                {
                  selected_buffer = &stream->buffers[selected_index];
                  if(buffer->block_offset >
                     selected_buffer->block_offset)
                    {
                      selected_index = buffer_index;
                    }
                }
            }

          buffer_count = stream->buffer_count;
        }
    }

  if(wait_for_buffer != 0 && selected_index != -1)
    {
      result = WaitIO(stream->buffers[selected_index].io_request);
      if(result < 0)
        {
          stream->last_error = result;
          block_file_last_error = result;
          return result;
        }
    }

  return 0;
}

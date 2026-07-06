#include "filefunctions.h"

#include "async_stream_runtime.h"

/* Reconstructed from the destructor tail in the original function at 0x00032778. */
int
dispose_async_block_stream(AsyncBlockStream *stream)
{
  int index;
  AsyncStreamBuffer *buffer;

  if(stream == 0)
    {
      return 0;
    }


  index = 0;
  while(index < stream->buffer_count)
    {
      buffer = &stream->buffers[index];
      if(buffer->io_request > 0)
        {
          if(buffer->state == ASYNC_STREAM_BUFFER_READ_PENDING)
            {
              AbortIO(buffer->io_request);
              WaitIO(buffer->io_request);
            }
          DeleteItem(buffer->io_request);
          buffer->io_request = 0;
          buffer->io_request_record = 0;
        }
      index++;
    }
  if(stream->file_item > 0)
    {
      CloseDiskFile(stream->file_item);
      stream->file_item = 0;
    }

  return dispose_async_block_stream_storage(stream);
}

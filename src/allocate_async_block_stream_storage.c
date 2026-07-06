/* Reconstructed from the original function at 0x000325E4. */

#include "async_stream_runtime.h"
#include "memory_manager_runtime.h"
#include "string.h"

AsyncBlockStream *
allocate_async_block_stream_storage(int buffer_byte_count,
                                    int requested_buffer_count)
{
  int index;
  int metadata_byte_count;
  AsyncBlockStream *stream;
  AsyncStreamBuffer *buffer;
  unsigned char *candidate;

  if(buffer_byte_count <= 0 || requested_buffer_count <= 0 ||
     (unsigned int)requested_buffer_count >
       (0x7FFFFFFFU - sizeof(AsyncBlockStream)) /
       (sizeof(AsyncStreamBuffer) + (unsigned int)buffer_byte_count))
    {
      return 0;
    }

  metadata_byte_count =
    sizeof(AsyncBlockStream) +
    requested_buffer_count * sizeof(AsyncStreamBuffer);
  stream = (AsyncBlockStream *)MEM_NewPointer(
    requested_buffer_count * buffer_byte_count + metadata_byte_count,
    MEMTYPE_DMA,
    MEM_POINTER_OPTIONS_DEFAULT);

  if(stream == 0)
    {
      stream = (AsyncBlockStream *)MEM_NewPointer(
        metadata_byte_count,
        MEMTYPE_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
      if(stream == 0)
        {
          return 0;
        }

      memset(stream, 0, metadata_byte_count);

      index = 0;
      stream->buffer_count = requested_buffer_count;
      stream->buffers = (AsyncStreamBuffer *)(stream + 1);

      while(index < requested_buffer_count)
        {
          candidate = (unsigned char *)MEM_NewPointer(
            buffer_byte_count,
            MEMTYPE_DMA,
            MEM_POINTER_OPTIONS_DEFAULT);
          buffer = &stream->buffers[index];
          buffer->allocation_base = candidate;
          if(candidate == 0)
            {
              stream->buffer_count = index;
              break;
            }
          index++;
          buffer->data = candidate;
        }

      if(stream->buffer_count < 1)
        {
          dispose_async_block_stream_storage(stream);
          return 0;
        }
    }
  else
    {
      memset(stream, 0, metadata_byte_count);
      index = 0;
      stream->buffer_count = requested_buffer_count;
      stream->buffers = (AsyncStreamBuffer *)(stream + 1);

      while(index < requested_buffer_count)
        {
          buffer = &stream->buffers[index];
          buffer->allocation_base = stream;
          if(index == 0)
            {
              candidate = (unsigned char *)(
                stream->buffers + requested_buffer_count);
            }
          else
            {
              candidate = stream->buffers[index - 1].data +
                          buffer_byte_count;
            }
          index++;
          buffer->data = candidate;
        }
    }

  return stream;
}

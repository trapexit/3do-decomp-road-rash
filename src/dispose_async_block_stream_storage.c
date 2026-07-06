/* Reconstructed from the original function at 0x0003271C. */

#include "async_stream_runtime.h"
#include "memory_manager_runtime.h"

int
dispose_async_block_stream_storage(AsyncBlockStream *stream)
{
  int index;
  AsyncStreamBuffer *buffer;

  index = 0;
  while(index < stream->buffer_count)
    {
      buffer = &stream->buffers[index];
      if(buffer->allocation_base != stream)
        {
          MEM_DisposePointer(buffer->allocation_base);
        }
      index++;
    }

  return MEM_DisposePointer(stream);
}

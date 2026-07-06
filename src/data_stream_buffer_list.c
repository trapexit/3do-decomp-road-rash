#include "datastream.h"
#include "limits.h"

#include "memory_manager_runtime.h"

#define STREAM_BUFFER_ALIGNMENT 4
#define STREAM_BUFFER_ALIGNMENT_MASK (STREAM_BUFFER_ALIGNMENT - 1)

DSDataBufPtr
create_data_stream_buffer_list(long buffer_count,
                               long buffer_size)
{
  DSDataBufPtr first_buffer;
  DSDataBufPtr buffer;
  DSDataBufPtr next_buffer;
  long entry_size;
  long final_buffer_index;
  long buffer_index;

  if(buffer_count <= 0 || buffer_size <= 0 ||
     buffer_size > INT_MAX - STREAM_BUFFER_ALIGNMENT_MASK -
                     (long)sizeof(DSDataBuf))
    {
      return 0;
    }

  entry_size = (buffer_size + STREAM_BUFFER_ALIGNMENT_MASK) &
               ~STREAM_BUFFER_ALIGNMENT_MASK;
  entry_size += sizeof(DSDataBuf);
  if(buffer_count > INT_MAX / entry_size)
    {
      return 0;
    }
  first_buffer = (DSDataBufPtr)MEM_NewPointer(
    entry_size * buffer_count, MEMTYPE_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(first_buffer == 0)
    {
      return 0;
    }

  buffer = first_buffer;
  buffer_index = 0;
  final_buffer_index = buffer_count - 1;
  if(final_buffer_index > 0)
    {
      do
        {
          next_buffer = (DSDataBufPtr)((char *)buffer + entry_size);
          buffer->permanentNext = next_buffer;
          buffer->next = next_buffer;
          buffer = next_buffer;
          buffer_index++;
        } while(final_buffer_index > buffer_index);
    }

  buffer->permanentNext = 0;
  buffer->next = 0;
  return first_buffer;
}


int32
dispose_data_stream_buffer_list(DSDataBufPtr buffer_list)
{
  return MEM_DisposePointer(buffer_list);
}

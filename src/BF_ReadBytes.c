#include "byte_file_runtime.h"

int
BF_ReadBytes(ByteFileReader *reader,
             void           *destination,
             int             byte_count,
             int             position)
{
  ByteFileAsyncReadState read_state;

  if(BF_StartAsyncRead(reader, destination, byte_count, position,
                       &read_state) == 0)
    {
      return 0;
    }
  return BF_FinishAsyncRead(&read_state);
}

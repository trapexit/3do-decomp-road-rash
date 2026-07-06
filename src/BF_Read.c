#include "byte_file_runtime.h"





int
BF_Read(ByteFileReader *reader,
        void           *destination,
        int             byte_count)
{
  volatile int result;

  result = BF_ReadBytes(reader, destination, byte_count,
                        reader->position);
  return result;
}

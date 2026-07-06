#include "music_file_runtime.h"

int
read_music_file_bytes(ByteFileReader *reader,
                      void           *destination,
                      int             byte_count)
{
  int result;

  result = BF_Read(reader, destination, byte_count);
  if(result >= 0)
    {
      result = 0;
    }
  return result;
}

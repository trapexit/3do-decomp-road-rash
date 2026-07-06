#include "byte_file_runtime.h"
#include "score_program_runtime.h"

int
read_score_chunk_bytes(ByteFileReader *reader,
                       void           *destination,
                       int             byte_count,
                       int            *bytes_read)
{
  int result;
  int status;

  status = 0;
  result = BF_Read(reader, destination, byte_count);
  if(result < 0)
    {
      if(bytes_read != 0)
        {
          *bytes_read = 0;
        }
      return result;
    }

  if(result != byte_count)
    {
      if(reader->position != 0)
        {
          status = -1;
        }
      if(bytes_read != 0)
        {
          *bytes_read = result;
        }
    }

  return status;
}

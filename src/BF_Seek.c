#include "byte_file_runtime.h"

int
BF_Seek(ByteFileReader    *reader,
        int                offset,
        ByteFileSeekOrigin origin)
{
  int old_position;
  int position;

  old_position = reader->position;
  if(origin == BYTE_FILE_SEEK_SET)
    {
      reader->position = offset;
    }
  else if(origin == BYTE_FILE_SEEK_CURRENT)
    {
      reader->position += offset;
    }
  else if(origin == BYTE_FILE_SEEK_END)
    {
      reader->position = (int)reader->file_status.fs_ByteCount - offset;
    }

  position = reader->position;
  if(position < 0 ||
     position > (int)reader->file_status.fs_ByteCount)
    {
      reader->position = old_position;
      return BYTE_FILE_ERROR_SEEK_RANGE;
    }
  return 0;
}

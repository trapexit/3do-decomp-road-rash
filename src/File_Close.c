#include "filestreamfunctions.h"

int
File_Close(Stream *stream)
{
  CloseDiskStream(stream);
  return 0;
}

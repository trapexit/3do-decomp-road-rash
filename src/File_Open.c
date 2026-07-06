#include "filestreamfunctions.h"

Stream *
File_Open(char       *path,
          const char *mode)
{
  (void)mode;
  return OpenDiskStream(path, 0);
}

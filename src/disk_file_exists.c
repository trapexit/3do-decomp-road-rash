/* Reconstructed from the original function at 0x00030574. */

#include "disk_file_runtime.h"
#include "filefunctions.h"

int
disk_file_exists(char *path)
{
  int file;

  file = OpenDiskFile(path);
  if(file >= 0)
    {
      CloseDiskFile(file);
      return 1;
    }
  return 0;
}

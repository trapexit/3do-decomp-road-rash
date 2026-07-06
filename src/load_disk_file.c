#include "limits.h"
#include "string.h"
/* Reconstructed from the original function at 0x00030100. */

#include "driver.h"
#include "filesystem.h"
#include "io.h"
#include "platform.h"
#include "filefunctions.h"
#include "filesystem.h"

#include "disk_file_runtime.h"
#include "memory_manager_runtime.h"

#define DISK_FILE_BAD_POINTER_ERROR (-0x138B)
#define DISK_FILE_INVALID_SIZE_ERROR (-0x138A)
#define DISK_FILE_NO_MEMORY_ERROR (-0x1389)

int
load_disk_file(char  *path,
               char **buffer_out,
               int   *buffer_capacity_out)
{
  IOInfo info;
  FileStatus status;
  char *replacement_buffer;
  char *read_buffer;
  Item file;
  Item ior;
  IOReq *ior_request;
  int result;
  int io_error;
  int size;
  int logical_size;
  int delete_result;

  if(path == 0 || buffer_out == 0 || buffer_capacity_out == 0 ||
     *buffer_capacity_out < 0)
    {
      return DISK_FILE_BAD_POINTER_ERROR;
    }

  file = OpenDiskFile(path);
  if(file < 0)
    {
      return (int)file;
    }

  ior = CreateIOReq(0, 0, file, 0);
  if(ior < 0)
    {
      CloseDiskFile(file);
      return (int)ior;
    }

  ior_request = (IOReq *)LookupItem(ior);
  if(ior_request == 0)
    {
      DeleteItem(ior);
      CloseDiskFile(file);
      return DISK_FILE_BAD_POINTER_ERROR;
    }

  memset(&info, 0, sizeof(info));
  memset(&status, 0, sizeof(status));

  info.ioi_Command = CMD_STATUS;
  info.ioi_Recv.iob_Buffer = &status;
  info.ioi_Recv.iob_Len = sizeof(status);

  result = (int)DoIO(ior, &info);
  io_error = (int)ior_request->io_Error;
  if(result < 0 || io_error != 0)
    {
      DeleteItem(ior);
      CloseDiskFile(file);
      if(result < 0)
        {
          return result;
        }
      return io_error;
    }

  if(status.fs.ds_DeviceBlockSize == 0 ||
     status.fs.ds_DeviceBlockCount == 0 ||
     status.fs.ds_DeviceBlockCount >
     INT_MAX / status.fs.ds_DeviceBlockSize)
    {
      DeleteItem(ior);
      CloseDiskFile(file);
      return DISK_FILE_INVALID_SIZE_ERROR;
    }
  size = (int)(status.fs.ds_DeviceBlockSize *
               status.fs.ds_DeviceBlockCount);
  if(status.fs_ByteCount == 0 ||
     status.fs_ByteCount > (unsigned int)size)
    {
      DeleteItem(ior);
      CloseDiskFile(file);
      return DISK_FILE_INVALID_SIZE_ERROR;
    }
  logical_size = (int)status.fs_ByteCount;

  replacement_buffer = 0;
  read_buffer = *buffer_out;
  if(*buffer_out == 0 || *buffer_capacity_out < size)
    {
      replacement_buffer = (char *)MEM_NewPointer(
        size, MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
      if(replacement_buffer == 0)
        {
          DeleteItem(ior);
          CloseDiskFile(file);
          return DISK_FILE_NO_MEMORY_ERROR;
        }
      read_buffer = replacement_buffer;
    }

  memset(&info, 0, sizeof(info));
  info.ioi_Command = CMD_READ;
  info.ioi_Recv.iob_Buffer = read_buffer;
  info.ioi_Recv.iob_Len = size;

  result = (int)DoIO(ior, &info);
  io_error = (int)ior_request->io_Error;
  if(result < 0 || io_error != 0)
    {
      if(replacement_buffer != 0)
        {
          MEM_DisposePointer(replacement_buffer);
        }
      DeleteItem(ior);
      CloseDiskFile(file);
      if(result < 0)
        {
          return result;
        }
      return io_error;
    }
  if(ior_request->io_Actual < logical_size)
    {
      if(replacement_buffer != 0)
        {
          MEM_DisposePointer(replacement_buffer);
        }
      DeleteItem(ior);
      CloseDiskFile(file);
      return DISK_FILE_INVALID_SIZE_ERROR;
    }

  if(replacement_buffer != 0)
    {
      if(*buffer_out != 0)
        {
          result = (int)MEM_DisposePointer(*buffer_out);
          if(result < 0)
            {
              MEM_DisposePointer(replacement_buffer);
              DeleteItem(ior);
              CloseDiskFile(file);
              return result;
            }
        }
      *buffer_out = replacement_buffer;
      *buffer_capacity_out = size;
    }

  delete_result = (int)DeleteItem(ior);
  result = (int)CloseDiskFile(file);
  if(delete_result < 0)
    {
      return delete_result;
    }
  if(result >= 0)
    {
      return logical_size;
    }
  return result;
}


#undef DISK_FILE_BAD_POINTER_ERROR
#undef DISK_FILE_INVALID_SIZE_ERROR
#undef DISK_FILE_NO_MEMORY_ERROR

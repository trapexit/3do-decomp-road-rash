#include "limits.h"
#include "string.h"
/* Reconstructed from the original function at 0x000302D0. */

#include "disk_file_runtime.h"
#include "driver.h"
#include "filesystem.h"
#include "io.h"
#include "platform.h"
#include "filefunctions.h"

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;

__value_in_regs DivModResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);

int
write_disk_file(char *path,
                char *data,
                int   byte_count)
{
  IOInfo info;
  DeviceStatus status;
  Item file;
  Item ior;
  IOReq *ior_request;
  int result;
  int existing_size;
  int delete_result;

  if(path == 0 || data == 0 || byte_count < 0)
    {
      return -0x138B;
    }
  auto_maintain_nvram_filesystem();

  file = OpenDiskFile(path);
  if(file < 0)
    {
      file = CreateFile(path);
      if(file < 0)
        {
          auto_maintain_nvram_filesystem();
          return file;
        }

      file = OpenDiskFile(path);
      if(file < 0)
        {
          auto_maintain_nvram_filesystem();
          return file;
        }
    }

  ior = CreateIOReq(0, 0, file, 0);
  if(ior < 0)
    {
      CloseDiskFile(file);
      auto_maintain_nvram_filesystem();
      return ior;
    }


  ior_request = (IOReq *)LookupItem(ior);

  memset(&info, 0, sizeof(info));
  memset(&status, 0, sizeof(status));

  info.ioi_Command = CMD_STATUS;
  info.ioi_Recv.iob_Buffer = &status;
  info.ioi_Recv.iob_Len = sizeof(status);

  result = DoIO(ior, &info);
  if(result < 0 || ior_request->io_Error != 0)
    {
      if(result >= 0)
        {
          result = ior_request->io_Error;
        }
      DeleteItem(ior);
      CloseDiskFile(file);
      auto_maintain_nvram_filesystem();
      return result;
    }

  if(status.ds_DeviceBlockSize == 0 ||
     status.ds_DeviceBlockCount >
       INT_MAX / status.ds_DeviceBlockSize)
    {
      DeleteItem(ior);
      CloseDiskFile(file);
      auto_maintain_nvram_filesystem();
      return -0x138A;
    }
  existing_size = status.ds_DeviceBlockSize * status.ds_DeviceBlockCount;
  if((unsigned int)existing_size < (unsigned int)byte_count)
    {
      memset(&info, 0, sizeof(info));
      info.ioi_Command = FILECMD_ALLOCBLOCKS;
      info.ioi_Offset =
        unsigned_divide_with_remainder(status.ds_DeviceBlockSize,
                                       (unsigned int)(byte_count - existing_size +
                                                      status.ds_DeviceBlockSize - 1)).quotient;

      result = DoIO(ior, &info);
      if(result < 0 || ior_request->io_Error != 0)
        {
          if(result >= 0)
            {
              result = ior_request->io_Error;
            }
          DeleteItem(ior);
          CloseDiskFile(file);
          auto_maintain_nvram_filesystem();
          return result;
        }
    }

  memset(&info, 0, sizeof(info));
  info.ioi_Command = CMD_WRITE;
  info.ioi_Offset = 0;
  info.ioi_Send.iob_Buffer = data;
  info.ioi_Send.iob_Len = byte_count;
  info.ioi_Flags = 0;
  info.ioi_Flags2 = 0;

  result = DoIO(ior, &info);
  if(result < 0 || ior_request->io_Error != 0)
    {
      if(result >= 0)
        {
          result = ior_request->io_Error;
        }
      DeleteItem(ior);
      CloseDiskFile(file);
      auto_maintain_nvram_filesystem();
      return result;
    }
  if(ior_request->io_Actual != byte_count)
    {
      DeleteItem(ior);
      CloseDiskFile(file);
      auto_maintain_nvram_filesystem();
      return -0x138A;
    }

  memset(&info, 0, sizeof(info));
  info.ioi_Command = FILECMD_SETEOF;
  info.ioi_Offset = byte_count;

  result = DoIO(ior, &info);
  if(result >= 0 && ior_request->io_Error < 0)
    {
      result = ior_request->io_Error;
    }
  delete_result = DeleteItem(ior);
  if(result < 0)
    {
      CloseDiskFile(file);
      auto_maintain_nvram_filesystem();
      return result;
    }

  result = CloseDiskFile(file);
  if(delete_result < 0)
    {
      return delete_result;
    }
  if(result < 0)
    {
      auto_maintain_nvram_filesystem();
      return result;
    }

  auto_maintain_nvram_filesystem();
  return 0;
}


int
delete_disk_file(char *path)
{
  int result;

  result = DeleteFile(path);
  if(result >= 0)
    {
      result = 0;
    }
  return result;
}

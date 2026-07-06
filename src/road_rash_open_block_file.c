#include "item.h"
#include "io.h"

#include "road_rash_block_file.h"

int32
RoadRashOpenBlockFile(char        *name,
                      BlockFilePtr block_file)
{
  IOInfo status_request;
  Item io_request;
  IOReq *request;
  int32 result;

  block_file->fDevice = OpenDiskFile(name);
  if(block_file->fDevice < 0)
    {
      return block_file->fDevice;
    }

  io_request = CreateBlockFileIOReq(block_file->fDevice, 0);
  if(io_request < 0)
    {
      CloseDiskFile(block_file->fDevice);
      block_file->fDevice = -1;
      return io_request;
    }

  status_request.ioi_Command = CMD_STATUS;
  status_request.ioi_Flags = IO_QUICK;
  status_request.ioi_Unit = 0;
  status_request.ioi_Flags2 = 0;
  status_request.ioi_CmdOptions = 0;
  status_request.ioi_Recv.iob_Buffer = &block_file->fStatus;
  status_request.ioi_Recv.iob_Len = sizeof(block_file->fStatus);
  status_request.ioi_Send.iob_Buffer = 0;
  status_request.ioi_Send.iob_Len = 0;
  status_request.ioi_Offset = 0;

  result = DoIO(io_request, &status_request);
  request = (IOReq *)LookupItem(io_request);
  if(result >= 0 && request->io_Error < 0)
    {
      result = request->io_Error;
    }
  DeleteItem(io_request);
  if(result < 0)
    {
      CloseDiskFile(block_file->fDevice);
      block_file->fDevice = -1;
      return result;
    }
  return 0;
}

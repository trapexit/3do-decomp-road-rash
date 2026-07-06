#include "io.h"
#include "operror.h"
#include "stdio.h"
#include "string.h"
#include "byte_file_runtime.h"

#define IO_STATUS_RESULT_MASK 0xFF

Err
SF_AsyncRead(ByteFileReader *reader,
             void           *destination,
             int             sector_count,
             int             sector_number)
{
  IOInfo info;
  int result;

  memset(&info, 0, sizeof(info));
  info.ioi_Command = CMD_READ;
  info.ioi_Recv.iob_Buffer = destination;
  info.ioi_Recv.iob_Len =
    reader->file_status.fs.ds_DeviceBlockSize * sector_count;
  info.ioi_Offset = sector_number;

  if((CheckIO(reader->io_request_item) & IO_STATUS_RESULT_MASK) == 0)
    {
      WaitIO(reader->io_request_item);
    }

  result = SendIO(reader->io_request_item, &info);
  if(result < 0)
    {
      printf("SF_AsyncRead() Failed. Err - ");
      PrintfSysErr(result);
    }
  return result;
}

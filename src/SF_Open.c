#include "stdio.h"

#include "filesystem.h"
#include "io.h"
#include "operror.h"
#include "filefunctions.h"
#include "string.h"
#include "byte_file_runtime.h"
#include "platform.h"
#include "rw_semantic_data.h"

ByteFileReader *
SF_Open(const char     *path,
        ByteFileReader *reader,
        Item            reply_port_item)
{
  char error_text[BYTE_FILE_ERROR_TEXT_SIZE];
  IOInfo io_info;
  int result;

  reader->file_item = OpenDiskFile((char *)path);
  if(reader->file_item < 0)
    {
      GetSysErr(error_text, BYTE_FILE_ERROR_TEXT_SIZE, reader->file_item);
      printf("Can't open file %s, err %s\n", path, error_text);
      block_file_last_error = reader->file_item;
      return 0;
    }

  reader->io_request_item =
    SF_CreateIOReqItem(reader->file_item, reply_port_item);
  if(reader->io_request_item < 0)
    {
      GetSysErr(error_text, BYTE_FILE_ERROR_TEXT_SIZE,
                reader->io_request_item);
      printf("Can't create block file IO Req, err %s\n", error_text);
      CloseDiskFile(reader->file_item);
      block_file_last_error = reader->io_request_item;
      return 0;
    }

  reader->io_request = (IOReq *)LookupItem(reader->io_request_item);

  memset(&io_info, 0, sizeof(io_info));
  io_info.ioi_Command = CMD_STATUS;
  io_info.ioi_Flags = IO_QUICK;
  io_info.ioi_Recv.iob_Buffer = &reader->file_status;
  io_info.ioi_Recv.iob_Len = sizeof(FileStatus);
  result = DoIO(reader->io_request_item, &io_info);
  if(result < 0 || reader->io_request->io_Error < 0)
    {
      block_file_last_error =
        result < 0 ? result : reader->io_request->io_Error;
      DeleteItem(reader->io_request_item);
      CloseDiskFile(reader->file_item);
      return 0;
    }
  if(reader->file_status.fs.ds_DeviceBlockSize != BYTE_FILE_SECTOR_SIZE)
    {
      block_file_last_error = BADIOARG;
      DeleteItem(reader->io_request_item);
      CloseDiskFile(reader->file_item);
      return 0;
    }

  return reader;
}

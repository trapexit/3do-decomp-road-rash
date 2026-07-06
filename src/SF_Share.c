#include "stdio.h"

#include "operror.h"
#include "string.h"
#include "byte_file_runtime.h"
#include "platform.h"
#include "rw_semantic_data.h"

ByteFileReader *
SF_Share(ByteFileReader *source_reader,
         ByteFileReader *reader,
         Item            reply_port_item)
{
  char error_text[BYTE_FILE_ERROR_TEXT_SIZE];

  memcpy(reader, source_reader, BYTE_FILE_SHARED_PREFIX_SIZE);

  reader->file_item = OpenItem(source_reader->file_item, 0);
  if(reader->file_item < 0)
    {
      GetSysErr(error_text, BYTE_FILE_ERROR_TEXT_SIZE, reader->file_item);
      printf("Can't Open Item to share SectorFile, err %s\n", error_text);
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
      block_file_last_error = reader->io_request_item;
      CloseItem(reader->file_item);
      return 0;
    }

  reader->io_request = (IOReq *)LookupItem(reader->io_request_item);
  return reader;
}

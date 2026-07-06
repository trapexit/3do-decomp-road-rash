#include "io.h"

#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

int
SF_Read(ByteFileReader *reader,
        void           *destination,
        int             sector_count,
        int             sector_number)
{
  int result;
  int actual_sectors;

  result = SF_AsyncRead(reader, destination, sector_count, sector_number);
  if(result < 0)
    {
      block_file_last_error = result;
      return 0;
    }

  result = WaitIO(reader->io_request_item);
  if(result < 0 || reader->io_request->io_Error < 0)
    {
      block_file_last_error =
        result < 0 ? result : reader->io_request->io_Error;
      return 0;
    }
  actual_sectors = reader->io_request->io_Actual;
  if(actual_sectors < 0)
    {
      actual_sectors += BYTE_FILE_SECTOR_SIZE - 1;
    }
  return actual_sectors >> BYTE_FILE_SECTOR_SHIFT;
}

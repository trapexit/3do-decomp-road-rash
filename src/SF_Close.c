#include "rw_semantic_data.h"
#include "byte_file_runtime.h"
#include "filefunctions.h"

int
SF_Close(ByteFileReader *reader)
{
  int result;
  int close_result;
  BlockFileSectorCache *cache;

  cache = &block_file_sector_cache;
  if(cache->sector_buffer != 0 &&
     LockItem(cache->lock_item, 1) >= 0)
    {
      if(cache->file_item == reader->file_item)
        {
          cache->file_item = -1;
          cache->valid_bytes = 0;
        }
      UnlockItem(cache->lock_item);
    }

  result = DeleteItem(reader->io_request_item);
  close_result = CloseDiskFile(reader->file_item);
  return result < 0 ? result : close_result;
}

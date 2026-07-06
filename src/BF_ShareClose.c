#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

void
BF_ShareClose(ByteFileReader *reader)
{
  RwMemoryAllocator allocate_reader;
  RwMemoryReleaser release_reader;
  BlockFileSectorCache *cache;

  if(reader != 0)
    {
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
      DeleteItem(reader->io_request_item);
      CloseItem(reader->file_item);
      BF_GetHooks((int *)&allocate_reader, (int *)&release_reader);
      release_reader((int)reader, BYTE_FILE_READER_SIZE);
    }
}

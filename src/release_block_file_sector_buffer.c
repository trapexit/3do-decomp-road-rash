#include "item.h"
#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

void
release_block_file_sector_buffer(void)
{
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
  BlockFileSectorCache *cache;

  cache = &block_file_sector_cache;
  if(cache->sector_buffer != 0 &&
     (cache->flags & BYTE_FILE_EXTERNAL_BUFFER_FLAG) == 0)
    {
      BF_GetHooks((int *)&allocate, (int *)&release);
      release((int)cache->sector_buffer, BYTE_FILE_SECTOR_SIZE);
    }

  cache->sector_buffer = 0;
  if(cache->lock_item > 0)
    {
      DeleteItem(cache->lock_item);
    }
  cache->lock_item = 0;
  cache->file_item = -1;
  cache->sector_number = -1;
  cache->valid_bytes = 0;
  cache->flags &= ~BYTE_FILE_EXTERNAL_BUFFER_FLAG;
}

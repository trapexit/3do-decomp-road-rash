#include "operror.h"
#include "semaphore.h"

#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

int
BF_InitReader(void             *sector_buffer,
              RwMemoryAllocator allocate_hook,
              RwMemoryReleaser  release_hook)
{
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
  BlockFileSectorCache *cache;

  cache = &block_file_sector_cache;
  if(cache->sector_buffer != 0)
    {
      return 1;
    }

  BF_SetHooks(allocate_hook, release_hook);
  BF_GetHooks((int *)&allocate, (int *)&release);

  if(sector_buffer == 0 && cache->sector_buffer == 0)
    {
      cache->sector_buffer =
        (void *)allocate(BYTE_FILE_SECTOR_SIZE, 0);
      if(cache->sector_buffer == 0)
        {
          block_file_last_error = NOMEM;
          return -1;
        }
      cache->flags &= ~BYTE_FILE_EXTERNAL_BUFFER_FLAG;
    }
  else if(sector_buffer != 0)
    {
      cache->sector_buffer = sector_buffer;
      cache->flags |= BYTE_FILE_EXTERNAL_BUFFER_FLAG;
    }

  cache->lock_item = CreateSizedItem(
    MKNODEID(KERNELNODE, SEMAPHORENODE), 0, 0);
  if(cache->lock_item < 0)
    {
      block_file_last_error = cache->lock_item;
      release_block_file_sector_buffer();
      return BYTE_FILE_ERROR_LOCK_CREATE;
    }

  cache->file_item = -1;
  cache->sector_number = -1;
  cache->valid_bytes = 0;
  return 0;
}

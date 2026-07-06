/* Original address: 0x000440AC. */



#include "rw_semantic_data.h"

int
block_file_allocate_memory(int size,
                           int memory_type);
void
block_file_free_memory(int memory,
                       int size);

void
set_block_file_memory_callbacks(int allocator,
                                int releaser)
{
  if(allocator == 0)
    {
      allocator = (int)block_file_allocate_memory;
    }
  block_file_api_globals.allocate = (RwMemoryAllocator)allocator;

  if(releaser == 0)
    {
      releaser = (int)block_file_free_memory;
    }
  block_file_api_globals.release = (RwMemoryReleaser)releaser;
}

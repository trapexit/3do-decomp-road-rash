#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

void
BF_SetHooks(RwMemoryAllocator allocate_hook,
            RwMemoryReleaser  release_hook)
{
  if(allocate_hook != 0)
    {
      byte_file_memory_vectors.allocate = allocate_hook;
    }
  if(release_hook != 0)
    {
      byte_file_memory_vectors.release = release_hook;
    }
}

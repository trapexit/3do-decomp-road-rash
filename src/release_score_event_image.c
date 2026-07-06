/* General-purpose game deallocator callback. */

#include "memory_manager_runtime.h"

int
release_score_event_image(void *memory,
                          int   size)
{
  (void)size;
  return MEM_DisposePointer(memory);
}

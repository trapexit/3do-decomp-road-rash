/* General-purpose game allocator callback. */

#include "memory_manager_runtime.h"

void *
allocate_score_event_image(int size,
                           int flags)
{
  (void)flags;
  return MEM_NewPointer(size, MEMORY_REQUEST_ANY,
                        MEM_POINTER_OPTIONS_DEFAULT);
}

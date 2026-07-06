#include "memory_manager_runtime.h"
#include "string.h"

void *
allocate_text_memory(int size,
                     int flags)
{
  void *memory;

  memory = MEM_NewPointer(
    size, (uint32)flags & ~(MEMTYPE_FILL | MEMTYPE_FILLMASK),
    MEM_POINTER_OPTIONS_DEFAULT);
  if(memory != 0 && (flags & MEMTYPE_FILL) != 0)
    {
      memset(memory, flags & MEMTYPE_FILLMASK, size);
    }
  return memory;
}

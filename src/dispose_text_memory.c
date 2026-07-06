#include "mem.h"

#include "text_render_runtime.h"

int
dispose_text_memory(void *memory,
                    int   unused_size,
                    int   unused_flags)
{
  (void)unused_size;
  (void)unused_flags;
  if(memory != 0)
    {
      return MEM_DisposePointer(memory);
    }
  return 0;
}

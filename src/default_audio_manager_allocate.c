#include "stdlib.h"

int
default_audio_manager_allocate(int size,
                               int memory_type)
{
  (void)memory_type;
  return (int)malloc(size);
}

#include "stdlib.h"

void
default_audio_manager_release(int memory,
                              int size)
{
  (void)size;
  free((void *)memory);
}

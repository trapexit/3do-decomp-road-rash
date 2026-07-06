/* Advance an index in the game's two-buffer display ring. */

#include "display_cel_runtime.h"

int
advance_display_buffer_index(int current_index)
{
  current_index++;
  if(current_index >= DISPLAY_BUFFER_COUNT)
    {
      current_index = 0;
    }
  return current_index;
}

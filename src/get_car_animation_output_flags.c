/* Reconstructed from the original function at 0x0001850C. */



#include "rw_semantic_data.h"

int
get_car_animation_output_flags(int animation_index)
{
  int index;

  index = (signed char)animation_index;
  return car_animation_output_info[index];
}

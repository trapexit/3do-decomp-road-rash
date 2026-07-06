#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"

int
front_end_choose_random_rider_preview(int operation,
                                      int selection);

void
front_end_reset_random_rider_preview(void)
{
  if(random_preview_active[0] != 0)
    {
      front_end_choose_random_rider_preview(0, 0);
      random_preview_active[0] = 0;
    }
  apply_front_end_cel_descriptors(front_end_cel_group_046);
}

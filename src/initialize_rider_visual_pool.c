#include "intrusive_list.h"
#include "rider_visual_runtime.h"
#include "rw_semantic_data.h"

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);

IntrusiveListStorage *
initialize_rider_visual_pool(void)
{
  rider_visual_pool = (IntrusiveListStorage *)create_intrusive_object_pool(
    sizeof(RiderVisualPoolSlot), RIDER_VISUAL_POOL_CAPACITY, 0);
  return rider_visual_pool;
}

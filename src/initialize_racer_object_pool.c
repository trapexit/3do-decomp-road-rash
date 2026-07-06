#include "ai_racer_runtime.h"
#include "rw_semantic_data.h"

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);

void
initialize_racer_object_pool(void)
{
  race_rider_state.object_pool = create_intrusive_object_pool(
    sizeof(AiRacerObject), race_rider_state.rider_count, 0);
}

#include "rw_semantic_data.h"

enum RacerStatePoolConstant
{
  RACER_STATE_POOL_ENTRY_SIZE = 0x1FC
};

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);

void
initialize_racer_state_pool(void)
{
  race_rider_state.state_pool = create_intrusive_object_pool(
    RACER_STATE_POOL_ENTRY_SIZE,
    race_rider_state.rider_count, 0);
}

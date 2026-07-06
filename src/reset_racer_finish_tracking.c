#include "rw_semantic_data.h"

void
reset_racer_finish_tracking(void)
{
  race_object_scheduler.pending_racer_count = 0;
  race_object_scheduler.player_finished = 0;
}

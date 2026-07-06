#include "rw_semantic_data.h"

void
reset_track_visibility_scan(void)
{
  track_visibility_scan.current_index =
    race_object_scheduler.visible_track_node_count - 1;
}

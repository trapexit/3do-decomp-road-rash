#include "bss_late_data.h"
#include "rw_semantic_data.h"

void
set_road_track_selector_override_enabled(RoadTrackTraversalState *traversal,
                                         int                      enabled)
{
  track_runtime_globals.selector_override = (unsigned char)enabled;
}

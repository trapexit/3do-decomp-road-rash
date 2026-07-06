#include "bss_late_data.h"
#include "rw_semantic_data.h"

void
commit_selected_road_track_channel(RoadTrackTraversalState *traversal)
{
  if(track_runtime_globals.selector_override == 0)
    {
      return;
    }

  traversal->channel_count = 1;
  traversal->clip_boundary_crossed = 0;
  traversal->selector_override_active = 0;

  if(traversal->active_channel != 0)
    {
      traversal->channels[0] = traversal->channels[1];
    }

  traversal->active_channel = 0;
  traversal->active_transition = 0;
}

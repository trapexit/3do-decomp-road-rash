#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

void
select_road_track_channel_override(int channel)
{
  track_runtime_globals.selector = channel;
  track_runtime_globals.selector_override = 1;

  if(gRoadTrackForwardTraversal.channel_count ==
     ROAD_TRACK_DUAL_CHANNEL_COUNT)
    {
      gRoadTrackForwardTraversal.active_channel = channel;
      gRoadTrackForwardTraversal.selector_override_active = 1;
    }

  if(gRoadTrackReverseTraversal.channel_count ==
     ROAD_TRACK_DUAL_CHANNEL_COUNT)
    {
      gRoadTrackReverseTraversal.active_channel = channel;
      gRoadTrackReverseTraversal.selector_override_active = 1;
    }
}

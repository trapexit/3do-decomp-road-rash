#include "memory_manager_runtime.h"
#include "track_segment_definition.h"

RoadTrackTransitionRuntime **
release_road_track_transition_runtime(RoadTrackGraphNode *node)
{
  if(node->transition_runtime != 0)
    {
      MEM_DisposePointer(node->transition_runtime);
    }
  node->transition_runtime = 0;
  return &node->transition_runtime;
}

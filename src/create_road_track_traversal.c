#include "bss_late_data.h"
#include "memory_manager_runtime.h"

RoadTrackTraversalState *
create_road_track_traversal(RoadTrackTraversalState *traversal,
                            RoadTrackGraphNode      *initial_clip,
                            int                      traversal_direction)
{
  int direction;

  direction = traversal_direction & TRACK_DIRECTION_BYTE_MASK;
  if(traversal == 0)
    {
      traversal = MEM_NewPointer(sizeof(*traversal), MEMORY_REQUEST_ANY,
                                 MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(traversal != 0)
    {
      traversal->track_position = 0;
      traversal->transition_status = ROAD_TRACK_TRANSITION_NONE;
      traversal->traversal_direction = (unsigned char)direction;
      traversal->active_channel = 0;
      traversal->channel_count = ROAD_TRACK_SINGLE_CHANNEL_COUNT;
      traversal->selector_override_active = 0;
      traversal->clip_boundary_crossed = 0;
      traversal->active_transition = 0;
      traversal->main_fork_channel = 0;
      traversal->alternate_fork_channel = 0;
      traversal->right_step = 0;
      traversal->left_step = 0;

      initialize_road_clip_playback_state(
        &traversal->channels[0], 0, 0, direction);
      initialize_road_clip_playback_state(
        &traversal->channels[1], 0, 1, direction);
      initialize_track_clip_traversal(
        &traversal->channels[0], initial_clip, 1);
    }

  return traversal;
}

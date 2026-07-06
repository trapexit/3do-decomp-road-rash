#include "bss_late_data.h"

TrackHazardTraversal *
create_track_hazard_traversal(TrackHazardTraversal *traversal,
                              TrackHazardTable     *table);

void
initialize_road_clip_playback_state(RoadClipPlaybackState *playback,
                                    RoadTrackGraphNode    *initial_clip,
                                    int                    lane_index,
                                    int                    traversal_direction)
{
  int direction;

  direction = traversal_direction & TRACK_DIRECTION_BYTE_MASK;
  playback->lane_index = lane_index;
  playback->active_clip = 0;
  playback->clip_position = 0;
  playback->center_position = 0;

  create_road_lane_width_traversal(&playback->lane_width_state, 0);
  create_road_object_traversal(&playback->object_state, 0);
  create_road_family_schedule_traversal(
    &playback->family_schedule_state, 0, direction);
  create_road_path_traversal(&playback->path_state, 0);
  create_road_section_traversal(&playback->section_state, 0);
  create_road_slope_geometry_traversal(
    &playback->slope_geometry_state, 0);
  create_road_terrain_traversal(&playback->terrain_state, 0);
  create_track_hazard_traversal(&playback->hazard_traversal, 0);

  if(initial_clip != 0)
    {
      initialize_track_clip_traversal(playback, initial_clip, 1);
    }
}

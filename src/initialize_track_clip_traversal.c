#include "bss_late_data.h"
#include "track_segment_definition.h"

int *
initialize_track_hazard_traversal(TrackHazardTraversal *traversal,
                                  TrackHazardTable     *table,
                                  int                   sample_count,
                                  int                   forward);

void
initialize_track_clip_traversal(RoadClipPlaybackState *playback,
                                RoadTrackGraphNode    *clip,
                                int                    forward)
{
  TrackSegmentDefinition *definition;

  forward &= TRACK_DIRECTION_BYTE_MASK;
  playback->active_clip = clip;
  if(forward == 0)
    {
      playback->clip_position =
        clip->extent.segment_sample_count
          << TRACK_POSITION_FRACTION_BITS;
    }
  else
    {
      playback->clip_position = 0;
    }

  definition = clip->payload.segment_definition;
  initialize_road_path_traversal(
    &playback->path_state, definition->path_resource, forward);
  initialize_road_lane_width_traversal(
    &playback->lane_width_state,
    definition->lane_width_resource, forward);
  initialize_road_object_traversal(
    &playback->object_state,
    definition->object_resource, forward);
  initialize_road_family_schedule_traversal(
    &playback->family_schedule_state,
    definition->family_schedule_resource,
    playback->clip_position);
  initialize_road_terrain_traversal(
    &playback->terrain_state,
    (const RoadTerrainResourceHeader *)definition->terrain_chunk,
    playback->clip_position);
  initialize_road_section_traversal(
    &playback->section_state, definition->section_resource, forward);
  initialize_road_slope_geometry_traversal(
    &playback->slope_geometry_state,
    definition->slope_geometry_resource, forward);
  initialize_track_hazard_traversal(
    &playback->hazard_traversal, definition->hazard_table,
    definition->sample_count, forward);
}

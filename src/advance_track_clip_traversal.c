#include "bss_late_data.h"
#include "track_segment_definition.h"

void
advance_track_hazard_traversal(TrackHazardTraversal *traversal,
                               int                   delta);

static
RoadClipAdvanceResult
classify_linked_road_track_node(const RoadTrackGraphNode *node)
{
  if(node == 0)
    {
      return ROAD_CLIP_ADVANCE_BLOCKED;
    }
  if(node->type == ROAD_TRACK_GRAPH_NODE_SEGMENT)
    {
      return ROAD_CLIP_ADVANCE_WITHIN_SEGMENT;
    }
  if(node->type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH ||
     node->type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
    {
      return ROAD_CLIP_ADVANCE_REQUIRES_TRANSITION;
    }
  return ROAD_CLIP_ADVANCE_BLOCKED;
}


RoadClipAdvanceResult
advance_track_clip_traversal(RoadClipPlaybackState *playback,
                             int                    delta)
{
  RoadTrackGraphNode *clip;
  RoadTrackGraphNode *next_clip;
  int old_position;
  int new_position;
  int clip_length;
  RoadClipAdvanceResult result;

  result = ROAD_CLIP_ADVANCE_WITHIN_SEGMENT;
  old_position = playback->clip_position;
  new_position = old_position + delta;
  clip = playback->active_clip;
  clip_length = clip->extent.segment_sample_count;

  if(new_position >=
     (clip_length << TRACK_POSITION_FRACTION_BITS))
    {
      next_clip = clip->primary_link;
      result = classify_linked_road_track_node(next_clip);
      if(result != ROAD_CLIP_ADVANCE_WITHIN_SEGMENT)
        {
          return result;
        }

      delta = new_position -
              (clip_length << TRACK_POSITION_FRACTION_BITS);
      initialize_track_clip_traversal(playback, next_clip, 1);
      new_position = delta;
      result = ROAD_CLIP_ADVANCE_CROSSED_SEGMENT;
    }
  else if(new_position < 0)
    {
      next_clip = clip->secondary_link;
      result = classify_linked_road_track_node(next_clip);
      if(result != ROAD_CLIP_ADVANCE_WITHIN_SEGMENT)
        {
          return result;
        }

      delta = -old_position;
      initialize_track_clip_traversal(playback, next_clip, 0);
      clip = playback->active_clip;
      new_position = delta +
                     (clip->extent.segment_sample_count <<
                      TRACK_POSITION_FRACTION_BITS);
      result = ROAD_CLIP_ADVANCE_CROSSED_SEGMENT;
    }

  playback->clip_position = new_position;
  advance_road_lane_width_traversal(
    &playback->lane_width_state, delta);
  advance_road_object_traversal(&playback->object_state, delta);
  advance_road_family_schedule_traversal(
    &playback->family_schedule_state, delta);
  advance_road_path_traversal(&playback->path_state, delta);
  advance_road_section_traversal(&playback->section_state, delta);
  advance_road_slope_geometry_traversal(
    &playback->slope_geometry_state, delta);
  advance_road_terrain_traversal(&playback->terrain_state, delta);
  advance_track_hazard_traversal(
    &playback->hazard_traversal, delta);

  return result;
}


void
seek_track_clip_traversal(RoadClipPlaybackState *playback,
                          int                    target_position)
{
  int delta;

  /* Bounded steps stop naturally if a clip link requires a transition. */
  delta = target_position - playback->clip_position;
  if(delta > 0)
    {
      while(delta >= TRACK_POSITION_UNIT)
        {
          advance_track_clip_traversal(playback, TRACK_POSITION_UNIT);
          delta -= TRACK_POSITION_UNIT;
        }
      advance_track_clip_traversal(playback, delta);
    }
  else if(delta < 0)
    {
      while(delta <= -TRACK_POSITION_UNIT)
        {
          advance_track_clip_traversal(playback, -TRACK_POSITION_UNIT);
          delta += TRACK_POSITION_UNIT;
        }
      advance_track_clip_traversal(playback, delta);
    }
}

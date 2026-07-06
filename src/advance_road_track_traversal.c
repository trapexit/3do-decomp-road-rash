#include "bss_late_data.h"
#include "rw_semantic_data.h"
#include "track_segment_definition.h"

/* Reconstructed from the original function at 0x00021E84. */

enum RoadTrackAdvanceConstants
{
  ROAD_TRACK_LANE_WIDTH_SCALE_NUMERATOR = 250,
  ROAD_TRACK_FORWARD_BRANCH_OVERRIDE_SPAN = 80,
  ROAD_TRACK_REVERSE_BRANCH_OVERRIDE_SPAN = 48
};

static
void
copy_clip_playback_state(RoadClipPlaybackState *destination,
                         RoadClipPlaybackState *source)
{
  int *destination_words;
  int *source_words;
  int word_index;

  destination_words = (int *)destination;
  source_words = (int *)source;
  for(word_index = 0;
      word_index < ROAD_CLIP_PLAYBACK_BYTES / sizeof(int);
      word_index++)
    {
      destination_words[word_index] = source_words[word_index];
    }
}


static
int
scale_lane_width_to_track_units(int value)
{
  int scaled;

  scaled = value * ROAD_TRACK_LANE_WIDTH_SCALE_NUMERATOR;
  if(scaled < 0)
    {
      scaled += TRACK_POSITION_FRACTION_MASK;
    }
  return scaled >> TRACK_POSITION_FRACTION_BITS;
}


static
void
sample_clip_path_curvature(RoadClipPlaybackState *playback,
                           int                   *out_curvature_step)
{
  int ignored_elevation_step;

  sample_road_path_steps(
    &playback->path_state, out_curvature_step,
    &ignored_elevation_step);
}


static
void
sample_clip_lane_widths(RoadClipPlaybackState *playback,
                        int                   *out_first_width,
                        int                   *out_second_width)
{
  sample_road_lane_widths(&playback->lane_width_state, out_first_width,
                          out_second_width);
}


static
void
apply_track_selector_override(RoadTrackTraversalState *traversal)
{
  if(track_runtime_globals.selector_override != 0)
    {
      traversal->active_channel = track_runtime_globals.selector;
      traversal->selector_override_active = 1;
    }
}


static
void
select_transition_channels(RoadTrackTraversalState *traversal,
                           RoadTrackGraphNode      *transition)
{
  traversal->active_channel = transition->alternate_fork;
  traversal->main_fork_channel = transition->main_fork;
  traversal->alternate_fork_channel = transition->alternate_fork;
  apply_track_selector_override(traversal);
}


static
void
align_dual_channel_centers(RoadTrackTraversalState *traversal,
                           int                      old_first_width,
                           int                      old_second_width)
{
  RoadClipPlaybackState *first_channel;
  RoadClipPlaybackState *second_channel;
  int base_center;
  int first_width;
  int second_width;

  first_channel = &traversal->channels[ROAD_TRACK_FIRST_CHANNEL];
  second_channel = &traversal->channels[ROAD_TRACK_SECOND_CHANNEL];
  base_center = first_channel->center_position;

  sample_clip_lane_widths(first_channel, &first_width, &second_width);
  first_channel->center_position =
    base_center -
    scale_lane_width_to_track_units(old_first_width - first_width);

  sample_clip_lane_widths(second_channel, &first_width, &second_width);
  second_channel->center_position =
    base_center +
    scale_lane_width_to_track_units(old_second_width - second_width);
}


static
int
enter_dual_channel_transition(RoadTrackTraversalState *traversal,
                              RoadTrackGraphNode      *transition,
                              int                      delta,
                              int                      old_position,
                              int                      old_first_width,
                              int                      old_second_width,
                              int                      forward)
{
  RoadClipPlaybackState *first_channel;
  RoadClipPlaybackState *second_channel;
  int first_curvature_step;
  int second_curvature_step;
  int position;

  first_channel = &traversal->channels[ROAD_TRACK_FIRST_CHANNEL];
  second_channel = &traversal->channels[ROAD_TRACK_SECOND_CHANNEL];

  if(forward)
    {
      traversal->transition_status =
        ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY;
    }
  else
    {
      traversal->transition_status =
        ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY;
    }
  traversal->channel_count = ROAD_TRACK_DUAL_CHANNEL_COUNT;
  traversal->track_position += delta;

  initialize_track_clip_traversal(first_channel,
                                  transition->primary_link, forward);
  initialize_track_clip_traversal(second_channel,
                                  transition->secondary_link, forward);

  if(forward)
    {
      position = traversal->track_position & TRACK_POSITION_FRACTION_MASK;
    }
  else
    {
      position = (old_position & TRACK_POSITION_FRACTION_MASK) + delta;
    }
  advance_track_clip_traversal(first_channel, position);
  advance_track_clip_traversal(second_channel, position);

  select_transition_channels(traversal, transition);
  align_dual_channel_centers(traversal, old_first_width,
                             old_second_width);

  sample_clip_path_curvature(first_channel, &first_curvature_step);
  sample_clip_path_curvature(second_channel, &second_curvature_step);
  if(forward)
    {
      traversal->left_step = 0;
      traversal->right_step =
        second_curvature_step - first_curvature_step;
    }
  else
    {
      traversal->right_step =
        first_curvature_step - second_curvature_step;
      traversal->left_step =
        second_curvature_step - first_curvature_step;
    }

  return 1;
}


static
int
enter_single_channel_transition(RoadTrackTraversalState *traversal,
                                RoadTrackGraphNode      *transition,
                                RoadClipPlaybackState   *current_channel,
                                int                      delta,
                                int                      old_first_width,
                                int                      old_second_width,
                                int                      forward)
{
  RoadClipPlaybackState *first_channel;
  RoadClipPlaybackState *second_channel;
  RoadClipPlaybackState *sample_channel;
  RoadTrackGraphNode *saved_clip;
  int saved_position;
  int old_channel;
  int first_width;
  int second_width;
  int adjustment;
  int target;

  first_channel = &traversal->channels[ROAD_TRACK_FIRST_CHANNEL];
  second_channel = &traversal->channels[ROAD_TRACK_SECOND_CHANNEL];
  old_channel = traversal->active_channel;
  saved_clip = current_channel->active_clip;
  saved_position = current_channel->clip_position;

  if(forward)
    {
      traversal->transition_status =
        ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY;
    }
  else
    {
      traversal->transition_status =
        ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY;
    }
  traversal->channel_count = ROAD_TRACK_SINGLE_CHANNEL_COUNT;
  traversal->track_position += delta;
  traversal->active_channel = ROAD_TRACK_FIRST_CHANNEL;
  traversal->selector_override_active = 0;

  if(old_channel != ROAD_TRACK_FIRST_CHANNEL)
    {
      copy_clip_playback_state(first_channel, second_channel);
    }

  initialize_track_clip_traversal(first_channel,
                                  transition->payload.single_link,
                                  forward);
  sample_channel = forward ? first_channel : second_channel;
  sample_clip_lane_widths(sample_channel, &first_width, &second_width);
  if(old_channel == ROAD_TRACK_FIRST_CHANNEL)
    {
      adjustment = -(old_first_width - first_width);
    }
  else
    {
      adjustment = old_second_width - second_width;
    }
  first_channel->center_position +=
    scale_lane_width_to_track_units(adjustment);

  target = saved_position + delta;
  if(forward)
    {
      target -= saved_clip->extent.segment_sample_count <<
                TRACK_POSITION_FRACTION_BITS;
    }
  advance_track_clip_traversal(first_channel, target);
  return 1;
}


static
void
enter_linked_dual_channel_transition(RoadTrackTraversalState *traversal,
                                     RoadTrackGraphNode      *transition,
                                     RoadTrackGraphNode      *clip,
                                     int                      new_position,
                                     int                      transition_status,
                                     int                      forward)
{
  RoadTrackTransitionRuntime *transition_runtime;
  RoadTrackTraversalState *cached_traversal;
  RoadClipPlaybackState *first_channel;
  RoadClipPlaybackState *second_channel;
  int center_delta;
  int transition_span;

  transition_runtime = transition->transition_runtime;
  cached_traversal = &transition_runtime->traversal;
  first_channel = &traversal->channels[ROAD_TRACK_FIRST_CHANNEL];
  second_channel = &traversal->channels[ROAD_TRACK_SECOND_CHANNEL];

  center_delta =
    cached_traversal->channels[ROAD_TRACK_SECOND_CHANNEL].center_position -
    cached_traversal->channels[ROAD_TRACK_FIRST_CHANNEL].center_position;
  if(transition->secondary_link == clip)
    {
      traversal->active_channel = ROAD_TRACK_SECOND_CHANNEL;
      copy_clip_playback_state(second_channel, first_channel);
      copy_clip_playback_state(
        first_channel,
        &cached_traversal->channels[ROAD_TRACK_FIRST_CHANNEL]);
      first_channel->family_schedule_state.traversal_direction =
        traversal->traversal_direction;
      advance_track_clip_traversal(
        first_channel, new_position & TRACK_POSITION_FRACTION_MASK);
      first_channel->center_position =
        second_channel->center_position - center_delta;
    }
  else
    {
      traversal->active_channel = ROAD_TRACK_FIRST_CHANNEL;
      copy_clip_playback_state(
        second_channel,
        &cached_traversal->channels[ROAD_TRACK_SECOND_CHANNEL]);
      second_channel->family_schedule_state.traversal_direction =
        traversal->traversal_direction;
      advance_track_clip_traversal(
        second_channel, new_position & TRACK_POSITION_FRACTION_MASK);
      second_channel->center_position =
        center_delta + first_channel->center_position;
    }

  track_runtime_globals.selector = traversal->active_channel;
  transition_span =
    transition->extent.transition_span >> TRACK_POSITION_FRACTION_BITS;
  if((!forward &&
      (traversal->traversal_direction == TRACK_TRAVERSAL_REVERSE ||
       transition_span > ROAD_TRACK_FORWARD_BRANCH_OVERRIDE_SPAN)) ||
     (forward &&
      (traversal->traversal_direction == TRACK_TRAVERSAL_FORWARD ||
       transition_span > ROAD_TRACK_REVERSE_BRANCH_OVERRIDE_SPAN)))
    {
      set_road_track_selector_override_enabled(traversal, 1);
    }

  traversal->channel_count = ROAD_TRACK_DUAL_CHANNEL_COUNT;
  traversal->active_transition = transition;
  traversal->left_step = cached_traversal->left_step;
  traversal->right_step = cached_traversal->right_step;
  traversal->main_fork_channel = cached_traversal->main_fork_channel;
  traversal->alternate_fork_channel =
    cached_traversal->alternate_fork_channel;
  traversal->selector_override_active = 1;
  traversal->clip_boundary_crossed = 0;
  traversal->transition_status = (unsigned char)transition_status;
}


int
advance_road_track_traversal(RoadTrackTraversalState *traversal,
                             int                      delta)
{
  RoadClipPlaybackState *first_channel;
  RoadClipPlaybackState *second_channel;
  RoadClipPlaybackState *current_channel;
  RoadClipPlaybackState *other_channel;
  RoadTrackGraphNode *clip;
  RoadTrackGraphNode *transition;
  RoadTrackTransitionRuntime *transition_runtime;
  int old_position;
  int new_position;
  int old_sample;
  int new_sample;
  RoadClipAdvanceResult advance_result;
  int channel_index;
  int transition_type;
  int clip_position;
  int first_width;
  int second_width;
  int first_curvature_step;
  int second_curvature_step;
  int corrected_step;

  if(delta == 0)
    {
      return 0;
    }

  first_channel = &traversal->channels[ROAD_TRACK_FIRST_CHANNEL];
  second_channel = &traversal->channels[ROAD_TRACK_SECOND_CHANNEL];
  old_position = traversal->track_position;
  new_position = old_position + delta;
  old_sample = old_position >> TRACK_POSITION_FRACTION_BITS;
  new_sample = new_position >> TRACK_POSITION_FRACTION_BITS;
  if(old_sample != new_sample)
    {
      traversal->transition_status = ROAD_TRACK_TRANSITION_NONE;
    }

  if(traversal->channel_count == ROAD_TRACK_DUAL_CHANNEL_COUNT &&
     delta < 0 && old_sample != new_sample)
    {
      sample_clip_path_curvature(first_channel, &first_curvature_step);
      sample_clip_path_curvature(second_channel, &second_curvature_step);
      corrected_step =
        traversal->right_step -
        (second_curvature_step - first_curvature_step);
      traversal->right_step = corrected_step;
      traversal->left_step -= corrected_step;
    }

  channel_index = traversal->active_channel;
  current_channel = &traversal->channels[channel_index];
  advance_result = advance_track_clip_traversal(current_channel, delta);
  if(advance_result != ROAD_CLIP_ADVANCE_WITHIN_SEGMENT)
    {
      if(advance_result == ROAD_CLIP_ADVANCE_CROSSED_SEGMENT)
        {
          traversal->clip_boundary_crossed = 1;
        }
      else if(advance_result ==
              ROAD_CLIP_ADVANCE_REQUIRES_TRANSITION)
        {
          sample_clip_lane_widths(current_channel, &first_width,
                                  &second_width);
          clip = current_channel->active_clip;
          if(delta > 0)
            {
              transition = clip->primary_link;
            }
          else
            {
              transition = clip->secondary_link;
            }
          traversal->active_transition = transition;
          transition_runtime = transition->transition_runtime;
          traversal->junction_progress =
            transition_runtime->branch_clearance_index;
          transition_type = transition->type;

          if(delta > 0 &&
             transition_type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
            {
              return enter_dual_channel_transition(
                traversal, transition, delta, old_position,
                first_width, second_width, 1);
            }
          if(delta > 0 &&
             transition_type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
            {
              return enter_single_channel_transition(
                traversal, transition, current_channel, delta,
                first_width, second_width, 1);
            }
          if(delta <= 0 &&
             transition_type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
            {
              return enter_single_channel_transition(
                traversal, transition, current_channel, delta,
                first_width, second_width, 0);
            }
          if(delta <= 0 &&
             transition_type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
            {
              return enter_dual_channel_transition(
                traversal, transition, delta, old_position,
                first_width, second_width, 0);
            }
          return 1;
        }
      else
        {
          return 1;
        }
    }

  if(traversal->channel_count == ROAD_TRACK_DUAL_CHANNEL_COUNT)
    {
      channel_index = traversal->active_channel;
      other_channel = &traversal->channels[1 - channel_index];
      advance_track_clip_traversal(other_channel, delta);

      if(old_sample != new_sample)
        {
          transition = traversal->active_transition;
          transition_type = transition->type;
          if(transition_type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
            {
              traversal->transition_status =
                ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_SPAN;
            }
          else if(transition_type ==
                  ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
            {
              traversal->transition_status =
                ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_SPAN;
            }

          if(delta <= 0)
            {
              current_channel = &traversal->channels[channel_index];
              clip = current_channel->active_clip;
              clip_position = current_channel->clip_position;
              if(transition_type ==
                 ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
                {
                  traversal->junction_progress += 1;
                  if((clip_position >> TRACK_POSITION_FRACTION_BITS) == 0)
                    {
                      traversal->transition_status =
                        ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY;
                    }
                }
              else if(transition_type ==
                      ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
                {
                  traversal->junction_progress -= 1;
                  if(transition->extent.transition_span <
                     (clip->extent.segment_sample_count <<
                      TRACK_POSITION_FRACTION_BITS) - clip_position)
                    {
                      traversal->clip_boundary_crossed = 1;
                      commit_selected_road_track_channel(traversal);
                      if(traversal->traversal_direction !=
                         TRACK_TRAVERSAL_REVERSE)
                        {
                          set_road_track_selector_override_enabled(
                            traversal, 0);
                        }
                    }
                }
            }
          else
            {
              sample_clip_path_curvature(
                first_channel, &first_curvature_step);
              sample_clip_path_curvature(
                second_channel, &second_curvature_step);
              traversal->left_step += traversal->right_step;
              traversal->right_step +=
                second_curvature_step - first_curvature_step;

              current_channel = &traversal->channels[channel_index];
              clip = current_channel->active_clip;
              clip_position = current_channel->clip_position;
              if(transition_type ==
                 ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
                {
                  traversal->junction_progress -= 1;
                  if(transition->extent.transition_span < clip_position)
                    {
                      traversal->clip_boundary_crossed = 1;
                      commit_selected_road_track_channel(traversal);
                      if(traversal->traversal_direction ==
                         TRACK_TRAVERSAL_REVERSE)
                        {
                          set_road_track_selector_override_enabled(
                            traversal, 0);
                        }
                    }
                }
              else if(transition_type ==
                      ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
                {
                  traversal->junction_progress += 1;
                  if(clip->extent.segment_sample_count -
                     (clip_position >> TRACK_POSITION_FRACTION_BITS) == 1)
                    {
                      traversal->transition_status =
                        ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY;
                    }
                }
            }

          if(traversal->clip_boundary_crossed != 0)
            {
              commit_selected_road_track_channel(traversal);
            }
        }
    }
  else if(old_sample != new_sample)
    {
      channel_index = traversal->active_channel;
      current_channel = &traversal->channels[channel_index];
      clip = current_channel->active_clip;
      clip_position = current_channel->clip_position;

      if(delta <= 0)
        {
          transition = clip->secondary_link;
          transition_type = transition->type;
          if(transition_type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
            {
              if((clip_position >> TRACK_POSITION_FRACTION_BITS) ==
                 (transition->extent.transition_span >>
                  TRACK_POSITION_FRACTION_BITS))
                {
                  transition_runtime = transition->transition_runtime;
                  traversal->junction_progress =
                    transition_runtime->branch_clearance_index -
                    (clip_position >> TRACK_POSITION_FRACTION_BITS);
                  enter_linked_dual_channel_transition(
                    traversal, transition, clip, new_position,
                    ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_SPAN, 0);
                }
            }
          else if(transition_type ==
                  ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH &&
                  (clip_position >> TRACK_POSITION_FRACTION_BITS) == 0)
            {
              traversal->transition_status =
                ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY;
            }
        }
      else
        {
          transition = clip->primary_link;
          transition_type = transition->type;
          if(transition_type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
            {
              if(clip->extent.segment_sample_count -
                 (clip_position >> TRACK_POSITION_FRACTION_BITS) == 1)
                {
                  traversal->transition_status =
                    ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY;
                }
            }
          else if(transition_type ==
                  ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH &&
                  clip->extent.segment_sample_count -
                  (clip_position >> TRACK_POSITION_FRACTION_BITS) ==
                  (transition->extent.transition_span >>
                   TRACK_POSITION_FRACTION_BITS))
            {
              transition_runtime = transition->transition_runtime;
              traversal->junction_progress =
                (clip_position >> TRACK_POSITION_FRACTION_BITS) -
                (clip->extent.segment_sample_count -
                 transition_runtime->branch_clearance_index);
              enter_linked_dual_channel_transition(
                traversal, transition, clip, new_position,
                ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_SPAN, 1);
            }
        }

      if(traversal->clip_boundary_crossed != 0)
        {
          commit_selected_road_track_channel(traversal);
        }
    }

  traversal->track_position += delta;
  return 1;
}

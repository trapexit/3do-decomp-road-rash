#include "road_segment_runtime.h"

enum RoadSegmentRuntimeLocalValue
{
  ROAD_SEGMENT_INDEX_MASK = ROAD_SEGMENT_COUNT - 1,
  ROAD_SEGMENT_TRANSITION_MINIMUM = -128
};

static
void
set_previous_lane_links(RoadSegmentLaneRuntime *lane,
                        RoadSegmentLaneRuntime *linked_lane)
{
  int link_index;

  for(link_index = 0;
      link_index < ROAD_SEGMENT_LINK_VARIANT_COUNT;
      link_index++)
    {
      lane->previous_links[link_index] = linked_lane;
    }
}


static
void
set_next_lane_links(RoadSegmentLaneRuntime *lane,
                    RoadSegmentLaneRuntime *linked_lane)
{
  int link_index;

  for(link_index = 0;
      link_index < ROAD_SEGMENT_LINK_VARIANT_COUNT;
      link_index++)
    {
      lane->next_links[link_index] = linked_lane;
    }
}


static
void
copy_selected_lanes_to_previous_links(RoadSegmentLaneRuntime   *lane,
                                      const RoadSegmentRuntime *segment)
{
  int link_index;

  for(link_index = 0;
      link_index < ROAD_SEGMENT_LINK_VARIANT_COUNT;
      link_index++)
    {
      lane->previous_links[link_index] =
        segment->selected_lanes[link_index];
    }
}


static
void
copy_selected_lanes_to_next_links(RoadSegmentLaneRuntime   *lane,
                                  const RoadSegmentRuntime *segment)
{
  int link_index;

  for(link_index = 0;
      link_index < ROAD_SEGMENT_LINK_VARIANT_COUNT;
      link_index++)
    {
      lane->next_links[link_index] =
        segment->selected_lanes[link_index];
    }
}


void
populate_and_link_road_segment_runtime(int                      track_position,
                                       RoadTrackTraversalState *traversal)
{
  RoadSegmentRuntime *segment;
  RoadSegmentRuntime *connected_segment;
  RoadSegmentRuntime *active_segment;
  RoadSegmentLaneRuntime *primary_lane;
  RoadSegmentLaneRuntime *secondary_lane;
  RoadSegmentLaneRuntime *connected_primary_lane;
  RoadSegmentLaneRuntime *connected_secondary_lane;
  RoadSegmentLaneRuntime *selected_lane;
  RoadSegmentLaneRuntime *opposite_lane;
  RoadClipPlaybackState *playback;
  int channel_count;
  int transition_type;
  int orientation;
  int channel_index;
  int transition_amount;
  unsigned int traversal_direction;

  begin_road_segment_runtime_update();
  segment = gRoadSegmentEffectWorkspace.segment_index.by_track_index[
    ((unsigned int)track_position >> TRACK_POSITION_FRACTION_BITS) &
    ROAD_SEGMENT_INDEX_MASK];
  segment->track_position =
    track_position & ~TRACK_POSITION_FRACTION_MASK;
  channel_count = traversal->channel_count;
  segment->lane_count = (unsigned char)channel_count;
  segment->transition_type = (signed char)traversal->transition_status;

  primary_lane = &segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
  secondary_lane = &segment->lanes[ROAD_TRACK_SECOND_CHANNEL];

  if(channel_count == ROAD_TRACK_SINGLE_CHANNEL_COUNT)
    {
      segment->right_step = 0;
      segment->left_step = 0;
      primary_lane->topology_flags = ROAD_LANE_TOPOLOGY_SINGLE;
      segment->selected_lanes[ROAD_TRACK_FIRST_CHANNEL] = primary_lane;
      segment->selected_lanes[ROAD_TRACK_SECOND_CHANNEL] = primary_lane;
      segment->selected_lanes[ROAD_SEGMENT_MAIN_FORK_LINK] = primary_lane;
      segment->selected_lanes[ROAD_SEGMENT_ALTERNATE_FORK_LINK] =
        primary_lane;
    }
  else
    {
      segment->right_step = traversal->right_step;
      segment->left_step = traversal->left_step;
      segment->orientation = (signed char)traversal->active_channel;
      transition_amount = traversal->junction_progress;
      if(transition_amount <= ROAD_SEGMENT_TRANSITION_MINIMUM)
        {
          transition_amount = ROAD_SEGMENT_TRANSITION_MINIMUM;
        }
      segment->transition_amount = (signed char)transition_amount;

      segment->selected_lanes[ROAD_TRACK_FIRST_CHANNEL] = primary_lane;
      segment->selected_lanes[ROAD_TRACK_SECOND_CHANNEL] = secondary_lane;
      segment->selected_lanes[ROAD_SEGMENT_MAIN_FORK_LINK] =
        segment->selected_lanes[traversal->main_fork_channel];
      segment->selected_lanes[ROAD_SEGMENT_ALTERNATE_FORK_LINK] =
        segment->selected_lanes[traversal->alternate_fork_channel];
      primary_lane->topology_flags = ROAD_LANE_TOPOLOGY_PRIMARY;
      secondary_lane->topology_flags = ROAD_LANE_TOPOLOGY_SECONDARY;
      segment->selected_lanes[ROAD_SEGMENT_MAIN_FORK_LINK]
      ->topology_flags |= ROAD_LANE_TOPOLOGY_MAIN_FORK;
      segment->selected_lanes[ROAD_SEGMENT_ALTERNATE_FORK_LINK]
      ->topology_flags |= ROAD_LANE_TOPOLOGY_ALTERNATE_FORK;
    }

  traversal_direction = traversal->traversal_direction;

  if(traversal_direction != TRACK_TRAVERSAL_REVERSE)
    {
      connected_segment = segment->previous;
      if(channel_count == ROAD_TRACK_SINGLE_CHANNEL_COUNT)
        {
          if(connected_segment->lane_count ==
             ROAD_TRACK_SINGLE_CHANNEL_COUNT)
            {
              copy_selected_lanes_to_next_links(
                primary_lane, connected_segment);
              connected_primary_lane =
                &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
              set_previous_lane_links(
                connected_primary_lane, primary_lane);
            }
          else
            {
              transition_type = segment->transition_type;
              if(transition_type ==
                 ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY)
                {
                  copy_selected_lanes_to_next_links(
                    primary_lane, connected_segment);
                  set_previous_lane_links(
                    &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL],
                    primary_lane);
                  set_previous_lane_links(
                    &connected_segment->lanes[ROAD_TRACK_SECOND_CHANNEL],
                    primary_lane);
                }
              else if(
                transition_type !=
                ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY &&
                transition_type !=
                ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY &&
                transition_type !=
                ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY)
                {
                  orientation = connected_segment->orientation;
                  selected_lane =
                    connected_segment->selected_lanes[orientation];
                  set_next_lane_links(primary_lane, selected_lane);
                  set_previous_lane_links(selected_lane, primary_lane);
                  opposite_lane = connected_segment->selected_lanes[
                    ROAD_TRACK_SECOND_CHANNEL - orientation];
                  set_previous_lane_links(opposite_lane, opposite_lane);
                }
            }
        }
      else if(connected_segment->lane_count ==
              ROAD_TRACK_SINGLE_CHANNEL_COUNT)
        {
          transition_type = segment->transition_type;
          if(transition_type ==
             ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY)
            {
              connected_primary_lane =
                &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
              copy_selected_lanes_to_previous_links(
                connected_primary_lane, segment);
              set_next_lane_links(primary_lane, connected_primary_lane);
              set_next_lane_links(secondary_lane, connected_primary_lane);
            }
          else if(
            transition_type !=
            ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY &&
            transition_type !=
            ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY &&
            transition_type !=
            ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY)
            {
              orientation = segment->orientation;
              selected_lane = &segment->lanes[orientation];
              connected_primary_lane =
                &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
              set_next_lane_links(selected_lane, connected_primary_lane);
              set_previous_lane_links(
                connected_primary_lane, selected_lane);
              opposite_lane = segment->selected_lanes[
                ROAD_TRACK_SECOND_CHANNEL - orientation];
              set_next_lane_links(opposite_lane, opposite_lane);
            }
        }
      else
        {
          connected_primary_lane =
            &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
          connected_secondary_lane =
            &connected_segment->lanes[ROAD_TRACK_SECOND_CHANNEL];
          set_next_lane_links(primary_lane, connected_primary_lane);
          set_previous_lane_links(connected_primary_lane, primary_lane);
          set_next_lane_links(secondary_lane, connected_secondary_lane);
          set_previous_lane_links(connected_secondary_lane, secondary_lane);
        }
    }
  else
    {
      connected_segment = segment->next;
      if(channel_count == ROAD_TRACK_SINGLE_CHANNEL_COUNT)
        {
          if(connected_segment->lane_count ==
             ROAD_TRACK_SINGLE_CHANNEL_COUNT)
            {
              connected_primary_lane =
                &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
              set_previous_lane_links(primary_lane, connected_primary_lane);
              set_next_lane_links(connected_primary_lane, primary_lane);
            }
          else
            {
              transition_type = segment->transition_type;
              if(transition_type ==
                 ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY)
                {
                  copy_selected_lanes_to_previous_links(
                    primary_lane, connected_segment);
                  set_next_lane_links(
                    &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL],
                    primary_lane);
                  set_next_lane_links(
                    &connected_segment->lanes[ROAD_TRACK_SECOND_CHANNEL],
                    primary_lane);
                }
              else if(
                transition_type !=
                ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY &&
                transition_type !=
                ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY &&
                transition_type !=
                ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY)
                {
                  orientation = connected_segment->orientation;
                  selected_lane =
                    connected_segment->selected_lanes[orientation];
                  set_previous_lane_links(primary_lane, selected_lane);
                  set_next_lane_links(selected_lane, primary_lane);
                  opposite_lane = connected_segment->selected_lanes[
                    ROAD_TRACK_SECOND_CHANNEL - orientation];
                  set_next_lane_links(opposite_lane, opposite_lane);
                }
            }
        }
      else if(connected_segment->lane_count ==
              ROAD_TRACK_SINGLE_CHANNEL_COUNT)
        {
          transition_type = segment->transition_type;
          if(transition_type ==
             ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY)
            {
              connected_primary_lane =
                &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
              copy_selected_lanes_to_next_links(
                connected_primary_lane, segment);
              set_previous_lane_links(primary_lane, connected_primary_lane);
              set_previous_lane_links(
                secondary_lane, connected_primary_lane);
            }
          else if(
            transition_type !=
            ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY &&
            transition_type !=
            ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY &&
            transition_type !=
            ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY)
            {
              orientation = segment->orientation;
              selected_lane = &segment->lanes[orientation];
              connected_primary_lane =
                &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
              set_previous_lane_links(selected_lane, connected_primary_lane);
              set_next_lane_links(connected_primary_lane, selected_lane);
              opposite_lane = &segment->lanes[
                ROAD_TRACK_SECOND_CHANNEL - orientation];
              set_previous_lane_links(opposite_lane, opposite_lane);
            }
        }
      else
        {
          connected_primary_lane =
            &connected_segment->lanes[ROAD_TRACK_FIRST_CHANNEL];
          connected_secondary_lane =
            &connected_segment->lanes[ROAD_TRACK_SECOND_CHANNEL];
          set_previous_lane_links(primary_lane, connected_primary_lane);
          set_next_lane_links(connected_primary_lane, primary_lane);
          set_previous_lane_links(secondary_lane, connected_secondary_lane);
          set_next_lane_links(connected_secondary_lane, secondary_lane);
        }
    }

  channel_index = traversal->active_channel;
  playback = &traversal->channels[channel_index];
  selected_lane = segment->selected_lanes[channel_index];
  populate_road_segment_lane_runtime(
    selected_lane, playback, traversal_direction);

  if(segment->lane_count > ROAD_TRACK_SINGLE_CHANNEL_COUNT)
    {
      channel_index = ROAD_TRACK_SECOND_CHANNEL - channel_index;
      playback = &traversal->channels[channel_index];
      selected_lane = segment->selected_lanes[channel_index];
      populate_road_segment_lane_runtime(
        selected_lane, playback, traversal_direction);
    }

  if(traversal_direction == TRACK_TRAVERSAL_REVERSE)
    {
      active_segment = segment;
    }
  else
    {
      active_segment = segment->previous;
    }

  transition_type = active_segment->transition_type;
  if(transition_type != -1)
    {
      if(active_segment->lane_count ==
         ROAD_TRACK_SINGLE_CHANNEL_COUNT)
        {
          reconcile_connected_road_lane_runtime(
            active_segment->selected_lanes[ROAD_TRACK_FIRST_CHANNEL]);
        }
      else if(transition_type !=
              ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY)
        {
          channel_index = traversal->active_channel;
          reconcile_connected_road_lane_runtime(
            active_segment->selected_lanes[channel_index]);
          channel_index = ROAD_TRACK_SECOND_CHANNEL - channel_index;
          reconcile_connected_road_lane_runtime(
            active_segment->selected_lanes[channel_index]);
        }
    }

  if(channel_count > ROAD_TRACK_SINGLE_CHANNEL_COUNT)
    {
      reconcile_road_lane_transition_profiles(segment);
    }
}

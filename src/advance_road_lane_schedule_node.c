#include "road_lane_schedule_runtime.h"
#include "track_segment_definition.h"

static
RoadTrackGraphLeafScheduleView *
road_lane_schedule_view(RoadSegmentLaneRuntime *lane)
{
  return (RoadTrackGraphLeafScheduleView *)lane->clip_node;
}


static
RoadSegmentLaneRuntime **
road_lane_schedule_slot(RoadSegmentLaneRuntime *lane,
                        int                     bucket_index)
{
  return &road_lane_schedule_view(lane)->bucket_lanes[bucket_index];
}


static
RoadSegmentRuntime *
road_lane_owner_segment(RoadSegmentLaneRuntime *lane)
{
  return (RoadSegmentRuntime *)lane->resource_handle;
}


static
int
road_lane_count(RoadSegmentLaneRuntime *lane)
{
  return (int)(signed char)road_lane_owner_segment(lane)->lane_count;
}


static
RoadSegmentLaneRuntime *
road_lane_transition_child(RoadSegmentLaneRuntime *lane)
{
  RoadSegmentRuntime *segment;
  int side;

  segment = road_lane_owner_segment(lane);
  side = (lane->topology_flags & ROAD_LANE_TOPOLOGY_PRIMARY) != 0;
  return segment->selected_lanes[side];
}


int
advance_road_lane_schedule_node(RoadLaneScheduleNode *schedule,
                                int                   delta)
{
  int old_fixed;
  int new_fixed;
  int old_cell;
  int bucket_index;
  int transition_type;
  RoadSegmentLaneRuntime *old_lane;
  RoadSegmentLaneRuntime *current_lane;
  RoadLaneScheduleNode *created;

  old_fixed = schedule->track_position;
  old_cell = old_fixed >> ROAD_LANE_SCHEDULE_FRACTION_BITS;
  new_fixed = old_fixed + delta;
  schedule->track_position = new_fixed;
  if(old_cell ==
     (new_fixed >> ROAD_LANE_SCHEDULE_FRACTION_BITS))
    {
      return (int)schedule;
    }

  bucket_index = schedule->bucket_index;
  old_lane = schedule->lane;
  *road_lane_schedule_slot(old_lane, bucket_index) = 0;
  transition_type = road_lane_owner_segment(old_lane)->transition_type;

  if(delta > 0)
    {
      if(transition_type ==
         ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY &&
         (old_lane->topology_flags &
          ROAD_LANE_TOPOLOGY_SECONDARY) != 0)
        {
          return release_road_lane_schedule_node(schedule);
        }

      current_lane = old_lane->previous_links[0];
      schedule->lane = current_lane;
      if(old_lane->clip_node != current_lane->clip_node)
        {
          schedule->track_position = 0;
        }

      if(old_lane == current_lane)
        {
          if(bucket_index != 0)
            {
              *road_lane_schedule_slot(old_lane, bucket_index) = old_lane;
            }
          else
            {
              *road_lane_schedule_slot(old_lane, 0) = 0;
              *road_lane_schedule_slot(old_lane, 1) = 0;
            }
          return release_road_lane_schedule_node(schedule);
        }

      *road_lane_schedule_slot(current_lane, bucket_index) = current_lane;
      if(road_lane_count(old_lane) >= road_lane_count(current_lane))
        {
          return (int)road_lane_owner_segment(current_lane);
        }

      created = create_road_lane_schedule_node(
        road_lane_transition_child(current_lane),
        schedule->track_position, bucket_index);
      if(created == 0)
        {
          return 0;
        }

      current_lane = created->lane;
      *road_lane_schedule_slot(current_lane, created->bucket_index) =
        current_lane;
      if(bucket_index == 0 ||
         transition_type ==
         ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY)
        {
          return (int)created;
        }
      *road_lane_schedule_slot(current_lane, 0) = current_lane;
      return (int)current_lane;
    }

  if(transition_type ==
     ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY &&
     (old_lane->topology_flags & ROAD_LANE_TOPOLOGY_SECONDARY) != 0)
    {
      return release_road_lane_schedule_node(schedule);
    }

  current_lane = old_lane->next_links[0];
  schedule->lane = current_lane;
  if(old_lane->clip_node != current_lane->clip_node)
    {
      schedule->track_position =
        (current_lane->clip_node->extent.segment_sample_count - 1) <<
          ROAD_LANE_SCHEDULE_FRACTION_BITS;
    }

  if(old_lane == current_lane)
    {
      if(bucket_index == 0)
        {
          *road_lane_schedule_slot(old_lane, 0) = old_lane;
        }
      else
        {
          *road_lane_schedule_slot(old_lane, 0) = 0;
          *road_lane_schedule_slot(old_lane, 1) = 0;
        }
      return release_road_lane_schedule_node(schedule);
    }

  *road_lane_schedule_slot(current_lane, bucket_index) = current_lane;
  if(road_lane_count(old_lane) >= road_lane_count(current_lane))
    {
      return (int)road_lane_owner_segment(current_lane);
    }

  created = create_road_lane_schedule_node(
    road_lane_transition_child(current_lane),
    schedule->track_position, bucket_index);
  if(created == 0)
    {
      return 0;
    }

  current_lane = created->lane;
  *road_lane_schedule_slot(current_lane, created->bucket_index) =
    current_lane;
  if(bucket_index != 0 ||
     transition_type ==
     ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY)
    {
      return (int)created;
    }

  *road_lane_schedule_slot(current_lane, 1) = current_lane;
  return (int)current_lane;
}

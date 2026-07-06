#include "track_segment_definition.h"

#include "bss_late_data.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"
#include "track_traversal_runtime.h"

enum RoadViewMotionConstant
{
  ROAD_VIEW_SIGN_SHIFT = 31,
  ROAD_VIEW_HALF_SHIFT = 1,
  ROAD_VIEW_QUARTER_SHIFT = 2,
  ROAD_VIEW_QUARTER_ROUNDING_BIAS = 3,
  ROAD_VIEW_FIXED_8_8_SHIFT = 8,
  ROAD_VIEW_FIXED_8_8_ROUNDING_BIAS = 0xFF,
  ROAD_VIEW_SIXTEENTH_SHIFT = 4,
  ROAD_VIEW_SIXTEENTH_ROUNDING_BIAS = 0x0F,
  ROAD_VIEW_TRANSITION_BLEND_SPAN = 0x10,
  ROAD_VIEW_LATERAL_LIMIT = 200,
  ROAD_VIEW_PLAYER_FINISHED_FLAG = 0x10,
  ROAD_VIEW_CAMERA_HEIGHT_MAXIMUM = 300,
  ROAD_VIEW_CAMERA_FOLLOW_MAXIMUM = 900,
  ROAD_VIEW_CAMERA_FOLLOW_STEP = 3,
  ROAD_VIEW_PROJECTION_HEIGHT_MINIMUM = 0x96
};

static
int
signed_half_rounded_down(int value)
{
  return (value +
          (int)((unsigned int)value >> ROAD_VIEW_SIGN_SHIFT)) >>
         ROAD_VIEW_HALF_SHIFT;
}


static
int
signed_quarter_toward_zero(int value)
{
  if(value < 0)
    {
      value += ROAD_VIEW_QUARTER_ROUNDING_BIAS;
    }
  return value >> ROAD_VIEW_QUARTER_SHIFT;
}


static
int
fixed_8_8_to_integer_toward_zero(int value)
{
  if(value < 0)
    {
      value += ROAD_VIEW_FIXED_8_8_ROUNDING_BIAS;
    }
  return value >> ROAD_VIEW_FIXED_8_8_SHIFT;
}


static
int
signed_sixteenth_toward_zero(int value)
{
  if(value < 0)
    {
      value += ROAD_VIEW_SIXTEENTH_ROUNDING_BIAS;
    }
  return value >> ROAD_VIEW_SIXTEENTH_SHIFT;
}


static
int
road_lane_center(const RoadSegmentLaneRuntime *lane)
{
  return signed_half_rounded_down(lane->road_left + lane->road_right);
}


static
int
transition_sample_offset(const RoadSegmentLaneRuntime *lane,
                         const RoadSegmentRuntime     *segment,
                         int                           current_tick)
{
  int transition_type;

  transition_type = segment->transition_type;
  if(transition_type ==
     ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY ||
     transition_type ==
     ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_SPAN)
    {
      return -((lane->clip_position >> TRACK_POSITION_FRACTION_BITS) + 1);
    }
  if(transition_type ==
     ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY ||
     transition_type ==
     ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_SPAN)
    {
      return lane->clip_node->extent.segment_sample_count -
             (lane->clip_position >> TRACK_POSITION_FRACTION_BITS);
    }
  return current_tick;
}


static
int
blend_transition_lane_center(const RoadSegmentRuntime *segment,
                             int                       center,
                             int                       sample_offset)
{
  RoadSegmentRuntime *nearby_segment;
  RoadSegmentLaneRuntime *nearby_lane;
  int nearby_center;
  int absolute_offset;
  int segment_index;
  int center_delta;

  absolute_offset = sample_offset;
  if(absolute_offset < 1)
    {
      absolute_offset = -absolute_offset;
    }
  if(absolute_offset >= ROAD_VIEW_TRANSITION_BLEND_SPAN)
    {
      return center;
    }

  segment_index =
    ((segment->track_position +
      (sample_offset << TRACK_POSITION_FRACTION_BITS)) >>
     TRACK_POSITION_FRACTION_BITS) &
    (ROAD_SEGMENT_COUNT - 1);
  nearby_segment =
    gRoadSegmentEffectWorkspace.segment_index
    .by_track_index[segment_index];
  nearby_lane =
    nearby_segment->selected_lanes[ROAD_TRACK_FIRST_CHANNEL];
  nearby_center = road_lane_center(nearby_lane);

  center_delta = absolute_offset * (center - nearby_center);
  return nearby_center + signed_sixteenth_toward_zero(center_delta);
}


void
update_road_view_motion(int current_tick)
{
  RendererMotionState *motion;
  RaceObjectSchedulerState *scheduler;
  RacerEntity *player;
  RacerEntity *player_state;
  RoadSegmentLaneRuntime *lane;
  RoadSegmentRuntime *segment;
  int lane_center;
  int sample_offset;
  int lateral_delta;
  int lateral_adjustment;
  int lateral_quarter;
  int projection_height;
  int track_position;
  int previous_track_position;
  int player_is_active;

  motion = &track_simulation_globals.motion_source;
  scheduler = &race_object_scheduler;
  player = (RacerEntity *)race_rider_state.player_object;
  player_state = player->paired_state;
  lane = player_state->track_segment;
  segment = (RoadSegmentRuntime *)lane->resource_handle;
  previous_track_position = motion->packed_track_position;

  lane_center = road_lane_center(lane);
  if(segment->lane_count == ROAD_TRACK_DUAL_CHANNEL_COUNT)
    {
      sample_offset = transition_sample_offset(
        lane, segment, current_tick);
      lane_center = blend_transition_lane_center(
        segment, lane_center, sample_offset);
    }

  lateral_delta =
    fixed_8_8_to_integer_toward_zero(player_state->position_x) -
    lane_center;
  lateral_adjustment =
    lateral_delta - signed_quarter_toward_zero(lateral_delta);
  lateral_quarter = lateral_delta - lateral_adjustment;
  if(lateral_quarter > ROAD_VIEW_LATERAL_LIMIT)
    {
      lateral_adjustment = lateral_delta - ROAD_VIEW_LATERAL_LIMIT;
    }
  else if(lateral_quarter < -ROAD_VIEW_LATERAL_LIMIT)
    {
      lateral_adjustment = lateral_delta + ROAD_VIEW_LATERAL_LIMIT;
    }

  player_is_active =
    (player->status_flags & ROAD_VIEW_PLAYER_FINISHED_FLAG) == 0;
  if(!player_is_active || input_thread_state.deferred_event_ticks != 0)
    {
      if(scheduler->camera_height_offset <
         ROAD_VIEW_CAMERA_HEIGHT_MAXIMUM)
        {
          scheduler->camera_height_offset++;
        }
      if(scheduler->camera_follow_distance <
         ROAD_VIEW_CAMERA_FOLLOW_MAXIMUM)
        {
          scheduler->camera_follow_distance +=
            ROAD_VIEW_CAMERA_FOLLOW_STEP;
        }
    }

  motion->projection_origin_x = lane_center + lateral_adjustment;
  projection_height =
    player_state->position_aux + scheduler->camera_height_offset;
  if(projection_height < ROAD_VIEW_PROJECTION_HEIGHT_MINIMUM)
    {
      projection_height = ROAD_VIEW_PROJECTION_HEIGHT_MINIMUM;
    }
  motion->projection_origin_y = projection_height;

  track_position =
    player_state->right_collision_bound - TRACK_POSITION_UNIT -
    scheduler->camera_follow_distance;
  motion->packed_track_position = track_position;
  motion->source_position_z =
    (track_position - previous_track_position) <<
      ROAD_VIEW_FIXED_8_8_SHIFT;
}

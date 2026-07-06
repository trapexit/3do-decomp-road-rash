#include "ai_collision_avoidance_runtime.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

typedef struct AiUnsignedDivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} AiUnsignedDivModResult;

enum AiRacerControlConstant
{
  AI_FIXED_FRACTION_BITS = 8,
  AI_FORWARD_SPEED_SMOOTH_SHIFT = 3,
  AI_FORWARD_SPEED_SMOOTH_BIAS = 7,
  AI_NEARBY_TRACK_DISTANCE = 0x500,
  AI_DIFFICULTY_THREE_QUARTERS = 3,
  AI_DIFFICULTY_FULL = 4,
  AI_DIFFICULTY_THREE_QUARTER_NUMERATOR = 3,
  AI_DIVIDE_BY_FOUR_SHIFT = 2,
  AI_DIVIDE_BY_FOUR_BIAS = 3,
  AI_FINISH_EVENT_RANDOM_DIVISOR = 0x701,
  AI_FINISH_EVENT_MARGIN = 0x100,
  AI_TRACK_STOP_TYPE_A = 3,
  AI_TRACK_STOP_TYPE_B = 4,
  AI_TRACK_START_BIAS = 0x80,
  AI_RACER_FINISHED_FLAG = 0x10,
  AI_RACER_VISIBLE_FLAGS = 3
};

__value_in_regs AiUnsignedDivModResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);
int
rand(void);
int
schedule_racer_finish_event(int track_item);
void
update_racer_motion_and_collisions(int racer_address);

static
int
absolute_nonpositive(int value)
{
  return value <= 0 ? -value : value;
}


static
int
divide_by_two_towards_zero(int value)
{
  return (value + (int)((unsigned int)value >>
                        (sizeof(int) * AI_FIXED_FRACTION_BITS - 1))) >> 1;
}


static
int
divide_by_four_towards_zero(int value)
{
  if(value < 0)
    {
      value += AI_DIVIDE_BY_FOUR_BIAS;
    }
  return value >> AI_DIVIDE_BY_FOUR_SHIFT;
}


static
int
divide_by_eight_towards_zero(int value)
{
  if(value < 0)
    {
      value += AI_FORWARD_SPEED_SMOOTH_BIAS;
    }
  return value >> AI_FORWARD_SPEED_SMOOTH_SHIFT;
}


static
void
smooth_forward_speed(AiRacerObject *racer,
                     AiRacerObject *peer,
                     int            old_track_position,
                     int            new_track_position)
{
  int delta;
  int speed;
  int peer_speed;
  int absolute_speed;
  int absolute_peer_speed;
  int step;

  delta = new_track_position - peer->position.track_position;
  if(absolute_nonpositive(delta) <= AI_NEARBY_TRACK_DISTANCE)
    {
      return;
    }

  speed = racer->velocity.forward;
  peer_speed = peer->velocity.forward;
  absolute_speed = absolute_nonpositive(speed);
  absolute_peer_speed = absolute_nonpositive(peer_speed);

  if(absolute_peer_speed < absolute_speed)
    {
      racer->velocity.forward = speed - divide_by_eight_towards_zero(speed);
    }
  else
    {
      step = absolute_peer_speed - absolute_speed;
      racer->velocity.forward = speed + divide_by_eight_towards_zero(step);
    }

  (void)old_track_position;
}


static
int
ai_movement_delta(AiRacerObject *racer)
{
  AiRacerObject *player;
  int adjusted_speed;
  int difficulty;

  if(racer->movement_delay > 0)
    {
      racer->movement_delay -= racer->time_step;
      return 0;
    }

  player = (AiRacerObject *)race_rider_state.player_object;
  if(player->rider_mode == 0)
    {
      return (racer->time_step * racer->target_speed) >>
             AI_FIXED_FRACTION_BITS;
    }

  difficulty = player_profiles[
    race_mode_state.active_player_index].level;
  if(difficulty == AI_DIFFICULTY_THREE_QUARTERS)
    {
      adjusted_speed = divide_by_four_towards_zero(
        racer->target_speed * AI_DIFFICULTY_THREE_QUARTER_NUMERATOR);
    }
  else if(difficulty == AI_DIFFICULTY_FULL)
    {
      adjusted_speed = racer->target_speed;
    }
  else
    {
      adjusted_speed = divide_by_two_towards_zero(racer->target_speed);
    }
  return (racer->time_step * adjusted_speed) >>
         AI_FIXED_FRACTION_BITS;
}


static
void
advance_ai_track_cursor(AiRacerObject *racer)
{
  AiUnsignedDivModResult divmod;
  AiTrackCursorNode *node;
  RoadTrackGraphNode *branch;
  int finish_limit;
  int branch_type;

  node = racer->cursor_node;
  finish_limit = node->segment_definition->finish_sample_offset <<
                 AI_FIXED_FRACTION_BITS;
  if(finish_limit != 0 && finish_limit <= racer->cursor_position)
    {
      schedule_racer_finish_event((int)racer->paired_state);
      divmod = unsigned_divide_with_remainder(
        AI_FINISH_EVENT_RANDOM_DIVISOR, (unsigned int)rand());
      racer->cursor_position = (int)divmod.remainder + finish_limit +
                               AI_FINISH_EVENT_MARGIN;
    }

  while((racer->cursor_node->sample_count << AI_FIXED_FRACTION_BITS) <=
        racer->cursor_position)
    {
      node = racer->cursor_node;
      racer->cursor_position -=
        node->sample_count << AI_FIXED_FRACTION_BITS;

      branch = node->branch;
      branch_type = branch->type;
      if(branch_type == ROAD_TRACK_GRAPH_NODE_SEGMENT)
        {
          racer->cursor_node = (AiTrackCursorNode *)branch;
        }
      else if(branch_type == ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH)
        {
          racer->cursor_node = (AiTrackCursorNode *)
                               (&branch->primary_link)[branch->main_fork];
        }
      else if(branch_type == ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH)
        {
          racer->cursor_node =
            (AiTrackCursorNode *)branch->payload.single_link;
        }
      else if(branch_type == AI_TRACK_STOP_TYPE_A ||
              branch_type == AI_TRACK_STOP_TYPE_B)
        {
          racer->cursor_position =
            (node->sample_count - 1) << AI_FIXED_FRACTION_BITS;
          racer->target_speed = 0;
        }
    }
}


static
int
maybe_switch_ai_track_lane(AiRacerObject *racer,
                           AiRacerObject *peer)
{
  AiTrackCursorNode *node;
  RoadSegmentLaneRuntime *previous_lane;
  RoadSegmentLaneRuntime *next_lane;
  RoadSegmentLaneRuntime *target_lane;
  RoadSegmentLaneRuntime *selected_lane;
  AiLaneResourcePosition *lane_resource;
  int start;
  int end;
  int position;
  int distance_to_next;
  int distance_to_previous;

  node = racer->cursor_node;
  previous_lane = node->previous_lane;
  next_lane = node->next_lane;
  if(previous_lane == 0 && next_lane == 0)
    {
      return 0;
    }

  start = previous_lane == 0 ? 0 : previous_lane->clip_position;
  end = next_lane == 0
              ? (node->sample_count << AI_FIXED_FRACTION_BITS) - 1
              : next_lane->clip_position;

  position = racer->cursor_position;
  if(start > position || position > end)
    {
      return (int)previous_lane;
    }

  if(previous_lane == 0 || next_lane == 0)
    {
      target_lane = next_lane == 0 ? previous_lane : next_lane;
    }
  else
    {
      distance_to_next = absolute_nonpositive(position - end);
      distance_to_previous = absolute_nonpositive(position - start);
      target_lane = distance_to_next >= distance_to_previous
                          ? previous_lane
                          : next_lane;
    }

  if(target_lane == 0)
    {
      return (int)previous_lane;
    }

  selected_lane = move_road_lane_by_offset(
    target_lane, position - target_lane->clip_position);
  lane_resource = (AiLaneResourcePosition *)selected_lane->resource_handle;
  racer->position.track_position = lane_resource->track_position +
                                   AI_TRACK_START_BIAS;
  racer->position.vertical_position = 0;
  racer->position.auxiliary_position = 0;
  racer->track_segment = selected_lane;
  racer->position.lateral_position = selected_lane->center_position;
  racer->cached_track_segment_b = selected_lane;
  racer->cached_track_segment_a = selected_lane;
  racer->cached_track_position_b = racer->position.track_position;
  racer->cached_track_position_a = racer->position.track_position;
  racer->cursor_node = (AiTrackCursorNode *)selected_lane->clip_node;
  racer->cursor_position = selected_lane->clip_position;

  select_ai_lane_target(racer);
  racer->position.lateral_position =
    racer->paired_state->decision_state->lane_reference_position;
  select_ai_collision_avoidance_target(racer);
  racer->position.lateral_position =
    racer->paired_state->decision_state->lane_reference_position;

  racer->velocity.lateral = 0;
  racer->velocity.vertical = 0;
  if((racer->status_flags & AI_RACER_FINISHED_FLAG) == 0)
    {
      racer->velocity.forward = racer->target_speed;
    }
  else
    {
      racer->velocity.forward = 0;
    }

  racer->render_object = racer->cached_render_object;
  racer->prepare_update = racer->cached_prepare_update;
  peer->render_object = peer->cached_state_render_object;
  peer->prepare_update = peer->cached_state_prepare_update;
  racer->status_flags |= AI_RACER_VISIBLE_FLAGS;
  racer->cursor_initialized = 1;

  sync_rider_state_from_object((RacerEntity *)racer);
  return update_racer_race_rank(peer);
}


int
ai_racer_control(AiRacerObject *racer)
{
  AiRacerObject *peer;
  int old_track_position;
  int new_track_position;
  int delta;

  peer = racer->paired_state;
  if(racer->cursor_initialized != 0)
    {
      old_track_position = racer->position.track_position;
      update_racer_motion_and_collisions((int)racer);
      new_track_position = racer->position.track_position;

      if(peer->owner->rider_mode == 0)
        {
          sync_rider_state_from_object((RacerEntity *)racer);
          if(peer->owner->rider_mode == 0)
            {
              refresh_racer_race_rank_if_due(peer);
            }
        }
      else
        {
          smooth_forward_speed(racer, peer, old_track_position,
                               new_track_position);
        }

      racer->accumulated_track_delta +=
        new_track_position - old_track_position;
      racer->elapsed_time += racer->time_step;
      return racer->elapsed_time;
    }

  if((racer->status_flags & AI_RACER_FINISHED_FLAG) == 0)
    {
      delta = ai_movement_delta(racer);
      racer->cursor_position += delta;
      racer->position.track_position += delta;
      peer->position.track_position = racer->position.track_position;

      advance_ai_track_cursor(racer);
      update_racer_finish_distance(peer);
    }

  return maybe_switch_ai_track_lane(racer, peer);
}

#include "ai_racer_runtime.h"
#include "rw_semantic_data.h"

enum AiRacerPacingConstant
{
  AI_RACER_PACING_UPDATE_TICKS = 60,
  AI_RACER_PACING_NEIGHBOR_COUNT = 3,
  AI_RACER_MINIMUM_TARGET_SPEED = 3000,
  AI_RACER_MAXIMUM_TARGET_SPEED = 30000
};

static
int
racer_uses_other_course_branch(AiRacerObject *racer,
                               AiRacerObject *player)
{
  AiTrackCursorNode *racer_cursor;
  AiTrackCursorNode *player_cursor;

  racer_cursor = racer->cursor_node;
  player_cursor = player->cursor_node;
  if((racer_cursor->node_flags & player_cursor->node_flags &
      ROAD_TRACK_GRAPH_NODE_BRANCH_MASK) != 0)
    {
      return 1;
    }
  return racer_cursor->segment_definition->parent_junction !=
         player_cursor->segment_definition->parent_junction;
}


static
void
apply_rank_speed_penalty(AiRacerObject *racer_state,
                         int            adjustment)
{
  AiRacerObject *racer;

  racer = racer_state->owner;
  racer->target_speed += adjustment;
  if(racer->target_speed < AI_RACER_MINIMUM_TARGET_SPEED)
    {
      racer->target_speed = AI_RACER_MINIMUM_TARGET_SPEED;
    }
}


static
void
apply_rank_speed_bonus(AiRacerObject *racer_state,
                       int            adjustment)
{
  AiRacerObject *racer;

  racer = racer_state->owner;
  racer->target_speed += adjustment;
  if(racer->target_speed > AI_RACER_MAXIMUM_TARGET_SPEED)
    {
      racer->target_speed = AI_RACER_MAXIMUM_TARGET_SPEED;
    }
}


void
adjust_ai_racer_pacing_by_rank(void)
{
  AiRacerObject *player;
  AiRacerObject *candidate;
  AiRacerObject *racer_state;
  AiRacerObject *ranked_racers[RACE_RIDER_COUNT];
  int player_rank_index;
  int rank;
  int adjustment_index;
  int rider_index;

  if((unsigned int)frame_tick <
     (unsigned int)race_rider_state.pacing_update_deadline)
    {
      return;
    }
  player = (AiRacerObject *)race_rider_state.player_object;
  if(player == 0)
    {
      return;
    }

  player_rank_index = (signed char)(player->paired_state->race_rank - 1);
  race_rider_state.pacing_update_deadline =
    frame_tick + AI_RACER_PACING_UPDATE_TICKS;

  for(rider_index = 0; rider_index < RACE_RIDER_COUNT; rider_index++)
    {
      ranked_racers[rider_index] = 0;
    }

  for(rider_index = 0; rider_index < RACE_RIDER_COUNT; rider_index++)
    {
      candidate =
        (AiRacerObject *)race_rider_state.rider_objects[rider_index];
      if(candidate != 0 && candidate->cursor_initialized == 0 &&
         racer_uses_other_course_branch(candidate, player))
        {
          racer_state =
            (AiRacerObject *)race_rider_state.rider_states[rider_index];
          rank = racer_state->race_rank;
          if(rank > 0 && rank <= RACE_RIDER_COUNT)
            {
              ranked_racers[rank - 1] = racer_state;
            }
        }
    }

  adjustment_index = 0;
  rider_index = player_rank_index - 1;
  while(rider_index >= 0)
    {
      racer_state = ranked_racers[rider_index];
      if(racer_state != 0)
        {
          apply_rank_speed_penalty(
            racer_state,
            race_rider_state.rank_penalties[adjustment_index]);
          adjustment_index++;
          if(adjustment_index >= AI_RACER_PACING_NEIGHBOR_COUNT)
            {
              break;
            }
        }
      rider_index--;
    }

  adjustment_index = 0;
  rider_index = player_rank_index + 1;
  while(rider_index < RACE_RIDER_COUNT)
    {
      racer_state = ranked_racers[rider_index];
      if(racer_state != 0)
        {
          apply_rank_speed_bonus(
            racer_state,
            race_rider_state.rank_bonuses[adjustment_index]);
          adjustment_index++;
          if(adjustment_index >= AI_RACER_PACING_NEIGHBOR_COUNT)
            {
              break;
            }
        }
      rider_index++;
    }
}

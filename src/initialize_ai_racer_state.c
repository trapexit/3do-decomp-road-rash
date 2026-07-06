#include "ai_collision_avoidance_runtime.h"
#include "racer_creation_runtime.h"

enum AiRacerInitializationValue
{
  AI_DIFFICULTY_COUNT = 5,
  AI_MAXIMUM_DIFFICULTY = AI_DIFFICULTY_COUNT - 1,
  AI_LANE_STATE_WORD_COUNT = 7,
  AI_INITIAL_ELAPSED_TICKS = 600,
  AI_DIFFICULTY_BASE_PERCENT = 90,
  AI_DIFFICULTY_STEP_PERCENT = 10,
  AI_PERCENT_SCALE_SHIFT = 8,
  AI_SCALE_SHIFT_SIX = 6,
  AI_SCALE_SHIFT_FIVE = 5,
  AI_SCALE_SHIFT_TWO = 2,
  AI_MULTIPLY_BY_FIVE_SHIFT = 2,
  AI_MULTIPLY_BY_FIFTEEN_SHIFT = 4,
  AI_MULTIPLY_BY_EIGHT_SHIFT = 3
};

static
int
initial_ai_track_delta(int difficulty)
{
  int percentage;
  int scaled;

  percentage = difficulty * AI_DIFFICULTY_STEP_PERCENT +
               AI_DIFFICULTY_BASE_PERCENT;
  scaled = ((percentage << AI_SCALE_SHIFT_SIX) +
            (percentage << AI_SCALE_SHIFT_FIVE) +
            (percentage << AI_SCALE_SHIFT_TWO)) >>
           AI_PERCENT_SCALE_SHIFT;
  scaled += scaled << AI_MULTIPLY_BY_FIVE_SHIFT;
  return ((scaled << AI_MULTIPLY_BY_FIFTEEN_SHIFT) - scaled) <<
         AI_MULTIPLY_BY_EIGHT_SHIFT;
}


void
initialize_ai_racer_state(AiRacerObject *racer)
{
  AiRacerObject *state_ai;
  RacerEntity *racer_base;
  RacerEntity *state;
  RiderRuntimeSlot *runtime;
  AiRacerDecisionState *decision_state;
  int difficulty;
  int lane_state_index;
  int tuning_value;

  difficulty = (unsigned char)
               player_profiles[race_mode_state.active_player_index].level;
  if(difficulty >= AI_DIFFICULTY_COUNT)
    {
      difficulty = AI_MAXIMUM_DIFFICULTY;
    }

  racer_base = (RacerEntity *)racer;
  state_ai = racer->paired_state;
  state = (RacerEntity *)state_ai;
  runtime = state->runtime;
  runtime->recovery_health_ceiling = runtime->maximum_health;
  runtime->current_health = runtime->maximum_health;

  tuning_value = runtime->level_tuning[difficulty];
  state->attack_style = tuning_value;
  state->queued_attack_style = tuning_value;
  state->attack_target_refresh_tick = 0;
  state->attack_contact_tick = 0;

  decision_state = (AiRacerDecisionState *)state->interaction_data;
  decision_state->lane_selection_state = 0;
  decision_state->row_marker = 0;
  for(lane_state_index = 0;
      lane_state_index < AI_LANE_STATE_WORD_COUNT;
      lane_state_index++)
    {
      decision_state->lane_state_words[lane_state_index] = 0;
    }

  racer_base->current_bike_health = racer_base->maximum_bike_health;
  select_ai_lane_target(racer);

  racer->position.lateral_position =
    decision_state->lane_reference_position;
  racer->elapsed_time = AI_INITIAL_ELAPSED_TICKS;
  racer->accumulated_track_delta = initial_ai_track_delta(difficulty);
  racer->target_speed = 0;

  state_ai->finish_distance =
    update_racer_finish_distance(state_ai);
  state_ai->race_rank = 0;
  state_ai->next_rank_update_tick = state->last_update_tick;

  update_rider_pacing_state((char *)racer);
}

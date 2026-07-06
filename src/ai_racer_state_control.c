#include "ai_racer_runtime.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"

enum AiRacerStateControlConstant
{
  AI_RACER_LATERAL_POSITION_LIMIT = 0xC8000,
  AI_RACER_LIFECYCLE_FLAG_MASK = 0xFF,
  AI_RACER_MODE_NORMAL = 0,
  AI_RACER_MODE_RECOVERY = 1,
  AI_RACER_FINISHED_FLAG = 0x10,
  AI_RACER_VISIBLE_FLAGS = 3,
  AI_RACER_FORWARD_SCALE_SHIFT_A = 7,
  AI_RACER_FORWARD_SCALE_SHIFT_B = 9,
  AI_RACER_FORWARD_SCALE_SHIFT_C = 12,
  AI_RACER_FORWARD_SCALE_ROUNDING = 0x1F,
  AI_RACER_FORWARD_SCALE_SHIFT = 5,
  AI_RACER_FIXED_FRACTION_BITS = 8,
  AI_RACER_TICKS_PER_SECOND = 60,
  AI_RACER_PERCENT_BASE = 100
};

int
remove_challenge_racer_pair(AiRacerObject *racer);
int
schedule_racer_finish_event(int racer_address);
void
update_passive_racer_motion_and_collisions(RacerEntity *racer);

static
int
clamp_ai_racer_lateral_position(int position)
{
  if(position < -AI_RACER_LATERAL_POSITION_LIMIT)
    {
      return -AI_RACER_LATERAL_POSITION_LIMIT;
    }
  if(position > AI_RACER_LATERAL_POSITION_LIMIT)
    {
      return AI_RACER_LATERAL_POSITION_LIMIT;
    }
  return position;
}


void
ai_racer_state_control(AiRacerObject *racer_state)
{
  AiRacerObject *racer;
  int finish_threshold;

  racer = racer_state->owner;
  if(racer->rider_mode != AI_RACER_MODE_NORMAL)
    {
      update_passive_racer_motion_and_collisions(
        (RacerEntity *)racer_state);
      refresh_racer_race_rank_if_due(racer_state);
    }

  if(racer_state->track_segment == 0)
    {
      return;
    }

  finish_threshold = track_runtime_globals.finish_threshold;
  if(finish_threshold == 0 ||
     racer_state->position.track_position < finish_threshold)
    {
      return;
    }

  if((racer_state->status_flags & AI_RACER_FINISHED_FLAG) == 0)
    {
      schedule_racer_finish_event((int)racer_state);
    }
}


int
racer_object_destroy(AiRacerObject *racer,
                     int            lifecycle_flags)
{
  AiRacerObject *rider_state;
  AiRacerObject *player;
  AiRacerObject *challenge;
  int forward_velocity;
  int track_delta;
  int scaled_delay;
  SignedDivisionResult division;

  rider_state = racer->paired_state;
  if(racer->cursor_initialized == 0)
    {
      return 0;
    }

  if((lifecycle_flags & AI_RACER_LIFECYCLE_FLAG_MASK) != 0)
    {
      racer->position.lateral_position =
        clamp_ai_racer_lateral_position(
          racer->position.lateral_position);
      return 0;
    }

  player = (AiRacerObject *)race_rider_state.player_object;
  if(player == racer)
    {
      return 0;
    }

  challenge = (AiRacerObject *)race_rider_state.challenge_object;
  if(challenge != 0 && rider_state == challenge->paired_state)
    {
      return remove_challenge_racer_pair(racer);
    }

  racer->movement_delay = 0;
  if(racer->rider_mode != AI_RACER_MODE_NORMAL)
    {
      if(racer->rider_mode == AI_RACER_MODE_RECOVERY)
        {
          forward_velocity = rider_state->velocity.forward;
          scaled_delay =
            (forward_velocity >> AI_RACER_FORWARD_SCALE_SHIFT_A) +
            (forward_velocity >> AI_RACER_FORWARD_SCALE_SHIFT_B) +
            (forward_velocity >> AI_RACER_FORWARD_SCALE_SHIFT_C);
          if(scaled_delay < 0)
            {
              scaled_delay += AI_RACER_FORWARD_SCALE_ROUNDING;
            }
          scaled_delay >>= AI_RACER_FORWARD_SCALE_SHIFT;
          racer->movement_delay +=
            scaled_delay * AI_RACER_TICKS_PER_SECOND;
        }

      track_delta = racer->position.track_position -
                    rider_state->position.track_position;
      if(track_delta <= 0)
        {
          track_delta = -track_delta;
        }
      scaled_delay = track_delta >> AI_RACER_FIXED_FRACTION_BITS;
      racer->movement_delay +=
        scaled_delay * AI_RACER_TICKS_PER_SECOND;
      reset_rider_to_normal_mode((RacerEntity *)racer);
    }

  division = signed_divide_with_remainder(
    (unsigned int)racer->elapsed_time,
    (unsigned int)racer->accumulated_track_delta <<
      AI_RACER_FIXED_FRACTION_BITS);
  racer->target_speed = division.quotient;

  if(racer->position.track_position > player->position.track_position)
    {
      division = signed_divide_with_remainder(
        AI_RACER_PERCENT_BASE,
        race_rider_state.pacing_percent_bounds[0] * division.quotient);
    }
  else
    {
      division = signed_divide_with_remainder(
        AI_RACER_PERCENT_BASE,
        race_rider_state.pacing_percent_bounds[1] * division.quotient);
    }
  racer->target_speed = division.quotient;

  racer->cursor_initialized = 0;
  racer->cached_render_object = racer->render_object;
  racer->cached_prepare_update = racer->prepare_update;
  racer->render_object = 0;
  racer->prepare_update = 0;
  racer->status_flags = (unsigned char)(
    racer->status_flags & (unsigned char)~AI_RACER_VISIBLE_FLAGS);

  rider_state->cached_state_prepare_update =
    rider_state->prepare_update;
  rider_state->cached_state_render_object =
    rider_state->render_object;
  rider_state->prepare_update = 0;
  rider_state->render_object = 0;
  racer->track_segment = 0;
  rider_state->track_segment = 0;
  return 0;
}


void
racer_state_destroy(AiRacerObject *racer_state,
                    int            lifecycle_flags)
{
  AiRacerObject *racer;

  racer = racer_state->owner;
  if(racer->cursor_initialized == 0)
    {
      return;
    }
  if((lifecycle_flags & AI_RACER_LIFECYCLE_FLAG_MASK) == 0)
    {
      return;
    }

  racer_state->position.lateral_position =
    clamp_ai_racer_lateral_position(
      racer_state->position.lateral_position);
}

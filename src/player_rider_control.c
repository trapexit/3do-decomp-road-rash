#include "event.h"

#include "racer_runtime.h"
#include "rider_contact_runtime.h"
#include "spatial_audio_event_queue.h"

typedef struct RiderRaceProgressRuntime
{
  unsigned char reserved_00[0x38];
  int limit;
  int current;
} RiderRaceProgressRuntime;

enum PlayerRiderControlConstant
{
  PLAYER_RIDER_MODE_NORMAL = 0,
  PLAYER_RIDER_MODE_RECOVERY = 1,
  PLAYER_RIDER_MODE_DUAL_SHIFT_RECOVERY = 2,
  PLAYER_RIDER_FINISHED_FLAG = 0x10,
  PLAYER_RIDER_FINISH_SPEED_THRESHOLD = 0x12C,
  PLAYER_RIDER_OUTCOME_EVENT = 0x7F,
  PLAYER_RIDER_OUTCOME_DELAY_TICKS = 0xB4,
  PLAYER_RIDER_ATTACK_AUDIO_CLUB = 9,
  PLAYER_RIDER_ATTACK_AUDIO_CHAIN = 8,
  PLAYER_RIDER_RECOVERY_KIND_DUAL_SHIFT = 6,
  PLAYER_RIDER_RECOVERY_ANIMATION = 0x26,
  PLAYER_RIDER_RECOVERY_SPEED_LIMIT = 0x3E8,
  PLAYER_RIDER_RECOVERY_LATERAL_OFFSET = 0x2328,
  PLAYER_RIDER_LONGITUDINAL_COAST = 0,
  PLAYER_RIDER_LONGITUDINAL_ACCELERATE = 1,
  PLAYER_RIDER_LONGITUDINAL_BRAKE = 2,
  PLAYER_RIDER_STEERING_NEUTRAL = 0,
  PLAYER_RIDER_STEERING_LEFT = 1,
  PLAYER_RIDER_STEERING_RIGHT = 2,
  PLAYER_RIDER_ATTACK_PRIMARY_A = 0x0A,
  PLAYER_RIDER_ATTACK_PRIMARY_B = 0x0B,
  PLAYER_RIDER_ATTACK_PRIMARY_C = 0x0C,
  PLAYER_RIDER_ATTACK_PRIMARY_D = 0x0D,
  PLAYER_RIDER_ATTACK_FOLLOWUP_A = 0x12,
  PLAYER_RIDER_ATTACK_FOLLOWUP_B = 0x13,
  PLAYER_RIDER_ATTACK_FOLLOWUP_C = 0x18,
  PLAYER_RIDER_ATTACK_FOLLOWUP_D = 0x19
};

void
racer_state_update(RacerEntity *rider);
int
update_rider_animation_and_challenge_state(RacerEntity *rider);
int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);
int
select_secondary_rider_animation(RiderAnimationRuntime *animation,
                                 int                    state);
int
get_race_boost_activation_cooldown_ticks(void);
void
deactivate_race_boost(RacerEntity *rider);
void
activate_race_boost_if_ready(RacerEntity *rider);
int
set_rider_longitudinal_control_mode(RacerEntity *rider,
                                    int          mode);
int
set_rider_steering_control_mode(RacerEntity *rider,
                                int          mode,
                                int          turn);
void
schedule_race_outcome_event(int event,
                            int delay_ticks);
int
schedule_racer_finish_event(RacerEntity *rider);

static
int
is_attack_primary_animation(int animation_state)
{
  return animation_state == PLAYER_RIDER_ATTACK_PRIMARY_A ||
         animation_state == PLAYER_RIDER_ATTACK_PRIMARY_B ||
         animation_state == PLAYER_RIDER_ATTACK_PRIMARY_C ||
         animation_state == PLAYER_RIDER_ATTACK_PRIMARY_D;
}


static
int
is_attack_followup_animation(int animation_state)
{
  return animation_state == PLAYER_RIDER_ATTACK_FOLLOWUP_A ||
         animation_state == PLAYER_RIDER_ATTACK_FOLLOWUP_B ||
         animation_state == PLAYER_RIDER_ATTACK_FOLLOWUP_C ||
         animation_state == PLAYER_RIDER_ATTACK_FOLLOWUP_D;
}


static
void
advance_player_race_progress(RacerEntity *state)
{
  RiderRaceProgressRuntime *progress;

  if(state->rider_mode == PLAYER_RIDER_MODE_RECOVERY)
    {
      return;
    }

  progress = (RiderRaceProgressRuntime *)state->paired_state->runtime;
  if(progress->current < progress->limit)
    {
      progress->current += state->time_step;
      if(progress->limit < progress->current)
        {
          progress->current = progress->limit;
        }
    }
}


static
void
update_attack_input(RacerEntity *rider,
                    RacerEntity *state,
                    unsigned int input)
{
  RiderAnimationRuntime *animation;
  RacerEntity *target;
  int animation_state;

  animation = &rider->animation;
  if((input & ControlC) != 0 &&
     is_rider_attack_animation(state) == false)
    {
      if((input & ControlDown) == 0)
        {
          rider->attack_style = rider->queued_attack_style;
        }
      else
        {
          rider->attack_style = RIDER_ATTACK_STYLE_KICK;
        }
      target = refresh_rider_attack_target(state);
      select_rider_attack_animation(
        state, target, (input & ControlUp) != 0);
    }

  animation_state = (signed char)animation->primary_state;
  if((input & ControlC) != 0 &&
     is_attack_primary_animation(animation_state))
    {
      select_secondary_rider_animation(animation, animation_state);
      if(rider->attack_style == RIDER_ATTACK_STYLE_CLUB)
        {
          queue_spatial_audio_event(
            PLAYER_RIDER_ATTACK_AUDIO_CLUB,
            &rider->position_z, &rider->velocity.forward);
        }
      else if(rider->attack_style == RIDER_ATTACK_STYLE_CHAIN)
        {
          queue_spatial_audio_event(
            PLAYER_RIDER_ATTACK_AUDIO_CHAIN,
            &rider->position_z, &rider->velocity.forward);
        }
    }
  else if(is_attack_primary_animation(
            animation->secondary_state))
    {
      select_secondary_rider_animation(animation, -1);
    }
  else if((input & ControlC) != 0 &&
          (input & ControlDown) == 0 &&
          is_attack_followup_animation(animation_state))
    {
      target = refresh_rider_attack_target(state);
      select_rider_attack_animation(state, target, 1);
    }

  if(is_rider_attack_animation(state) != false)
    {
      resolve_rider_attack_contact(state);
    }
}


static
int
update_dual_shift_recovery(RacerEntity *rider,
                           RacerEntity *state,
                           unsigned int input)
{
  int turn;

  turn = 0;
  if((input & ControlRightShift) == 0 ||
     (input & ControlLeftShift) == 0)
    {
      if((input & ControlRightShift) != 0)
        {
          turn = 1;
        }
      else if((input & ControlLeftShift) != 0)
        {
          turn = -1;
        }
      return turn;
    }

  if((input & ControlUp) != 0 &&
     state->rider_mode == PLAYER_RIDER_MODE_NORMAL &&
     state->velocity.forward < PLAYER_RIDER_RECOVERY_SPEED_LIMIT)
    {
      begin_rider_recovery(state, 0, 0,
                           PLAYER_RIDER_RECOVERY_KIND_DUAL_SHIFT);
      select_primary_rider_animation(
        &rider->animation, PLAYER_RIDER_RECOVERY_ANIMATION);
      select_secondary_rider_animation(
        &rider->animation, PLAYER_RIDER_RECOVERY_ANIMATION);
      state->rider_mode = PLAYER_RIDER_MODE_DUAL_SHIFT_RECOVERY;
      state->position_x -= PLAYER_RIDER_RECOVERY_LATERAL_OFFSET;
    }

  return turn;
}


static
void
update_longitudinal_input(RacerEntity *state,
                          unsigned int input)
{
  unsigned int elapsed_ticks;
  int cooldown_ticks;

  if((input & ControlB) == 0)
    {
      deactivate_race_boost(state);
    }
  else
    {
      if((race_input_latch_state.previous_input & ControlB) == 0)
        {
          cooldown_ticks = get_race_boost_activation_cooldown_ticks();
          elapsed_ticks = frame_tick -
                          (unsigned int)race_input_latch_state.previous_input_tick;
          if((unsigned int)cooldown_ticks > elapsed_ticks)
            {
              activate_race_boost_if_ready(state);
            }
          race_input_latch_state.previous_input_tick = (int)frame_tick;
        }
      set_rider_longitudinal_control_mode(
        state, PLAYER_RIDER_LONGITUDINAL_ACCELERATE);
    }

  if((input & ControlA) != 0)
    {
      set_rider_longitudinal_control_mode(
        state, PLAYER_RIDER_LONGITUDINAL_BRAKE);
    }
  else if((input & ControlB) == 0)
    {
      set_rider_longitudinal_control_mode(
        state, PLAYER_RIDER_LONGITUDINAL_COAST);
    }
  race_input_latch_state.previous_input = input;
}


void
player_rider_control(RacerEntity *rider)
{
  RacerEntity *state;
  unsigned int input;
  int turn;

  state = rider->owner;
  advance_player_race_progress(state);

  if(race_input_latch_state.event_latched != 0)
    {
      racer_state_update(rider);
      if((state->status_flags & PLAYER_RIDER_FINISHED_FLAG) == 0)
        {
          return;
        }
      if(state->velocity.forward <
         PLAYER_RIDER_FINISH_SPEED_THRESHOLD)
        {
          schedule_race_outcome_event(
            PLAYER_RIDER_OUTCOME_EVENT,
            PLAYER_RIDER_OUTCOME_DELAY_TICKS);
        }
      return;
    }

  if(state->rider_mode != PLAYER_RIDER_MODE_NORMAL)
    {
      if((rider->status_flags & PLAYER_RIDER_FINISHED_FLAG) != 0)
        {
          schedule_racer_finish_event(rider);
          schedule_race_outcome_event(
            PLAYER_RIDER_OUTCOME_EVENT,
            PLAYER_RIDER_OUTCOME_DELAY_TICKS);
        }
      racer_state_update(rider);
      return;
    }

  input = track_simulation_globals.control_bits;
  update_attack_input(rider, state, input);
  turn = update_dual_shift_recovery(rider, state, input);

  if((state->status_flags & PLAYER_RIDER_FINISHED_FLAG) != 0)
    {
      if(state->velocity.forward <
         PLAYER_RIDER_FINISH_SPEED_THRESHOLD)
        {
          schedule_race_outcome_event(
            PLAYER_RIDER_OUTCOME_EVENT,
            PLAYER_RIDER_OUTCOME_DELAY_TICKS);
        }
      deactivate_race_boost(state);
      set_rider_longitudinal_control_mode(
        state, PLAYER_RIDER_LONGITUDINAL_BRAKE);
    }
  else
    {
      update_longitudinal_input(state, input);
    }

  if((input & ControlLeft) != 0)
    {
      set_rider_steering_control_mode(
        state, PLAYER_RIDER_STEERING_LEFT, turn);
    }
  else if((input & ControlRight) != 0)
    {
      set_rider_steering_control_mode(
        state, PLAYER_RIDER_STEERING_RIGHT, turn);
    }
  else
    {
      set_rider_steering_control_mode(
        state, PLAYER_RIDER_STEERING_NEUTRAL, turn);
    }

  update_rider_animation_and_challenge_state(state);
}

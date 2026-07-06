/* Rider attack contact response reconstructed from the original function at 0x00005B24. */

#include "stdlib.h"

#include "division_helpers.h"
#include "rider_contact_runtime.h"
#include "spatial_audio_event_queue.h"

enum RiderAttackContactConstant
{
  RIDER_ATTACK_FRAME_ACTIVE_MASK = 0x00600000,
  RIDER_ATTACK_CONTACT_CLASS = 0x20,
  RIDER_ATTACK_RANDOM_MASK = 3,
  RIDER_ATTACK_TARGET_SWAP_ROLL = 3,
  RIDER_ATTACK_ALTERNATE_ANIMATION_ROLL = 2,
  RIDER_ATTACK_TARGET_SWAP_DELAY_TICKS = 300,
  RIDER_ATTACK_CONTACT_BASE_DELAY_TICKS = 30,
  RIDER_ATTACK_CONTACT_LEVEL_DELAY_TICKS = 6,
  RIDER_ATTACK_DIFFICULTY_ROLL_BASE = 6,
  RIDER_ATTACK_LONGITUDINAL_IMPULSE = 300,
  RIDER_ATTACK_TARGET_STEERING_IMPULSE = 3000,
  RIDER_ATTACK_SOURCE_STEERING_IMPULSE = 1000,
  RIDER_ATTACK_KICK_STEERING_IMPULSE = 8000,
  RIDER_ATTACK_RIDE_ANIMATION = 2,
  RIDER_ATTACK_ONE_ARM_ANIMATION = 6,
  RIDER_ATTACK_BLOCKED_PRIMARY_STATE = 9,
  RIDER_CONTACT_AUDIO_CLUB = 9,
  RIDER_CONTACT_AUDIO_CHAIN = 8,
  RIDER_CONTACT_RESPONSE_TARGET_SLOT = 2,
  RIDER_CONTACT_RESPONSE_SOURCE_SLOT = 4
};

int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);
int
select_secondary_rider_animation(RiderAnimationRuntime *animation,
                                 int                    state);

static
int
active_rider_profile_level(void)
{
  int profile_index;

  profile_index = race_mode_state.active_player_index;
  return player_profiles[profile_index].level;
}


static
void
clamp_rider_steering_angle(RacerEntity *rider)
{
  if(rider->steering_angle > rider->steering_limit)
    {
      rider->steering_angle = rider->steering_limit;
    }
  if(rider->steering_angle < -rider->steering_limit)
    {
      rider->steering_angle = -rider->steering_limit;
    }
}


void
resolve_rider_attack_contact(RacerEntity *rider)
{
  RacerEntity *state;
  RacerEntity *target;
  RacerEntity *contact_candidate;
  RiderInteractionRuntime *interaction;
  RiderAnimationRuntime *animation;
  RiderAnimationRuntime *target_animation;
  RacerEntity *player;
  RiderAttackStyle attack_style;
  SignedDivisionResult random_roll;
  int frame;
  int profile_level;
  boolean attacks_left;
  boolean attacks_right;

  state = rider->paired_state;
  interaction = state->interaction_data;
  animation = &state->animation;

  if((animation->current_frame->frame_flags[1] &
      RIDER_ATTACK_FRAME_ACTIVE_MASK) == 0 &&
     animation->primary_frame != RIDER_ATTACK_RIDE_ANIMATION)
    {
      return;
    }

  contact_candidate = refresh_rider_attack_target(rider);
  target = interaction->attack_target;

  player = (RacerEntity *)race_rider_state.player_object;
  if(rider == player || target == player)
    {
      attack_style = (RiderAttackStyle)state->attack_style;
      if(attack_style == RIDER_ATTACK_STYLE_CLUB)
        {
          queue_spatial_audio_event(
            RIDER_CONTACT_AUDIO_CLUB, &rider->position_z,
            &rider->velocity.forward);
        }
      else if(attack_style == RIDER_ATTACK_STYLE_CHAIN)
        {
          queue_spatial_audio_event(
            RIDER_CONTACT_AUDIO_CHAIN, &rider->position_z,
            &rider->velocity.forward);
        }
    }

  if(target != 0)
    {
      RacerEntity *target_state;

      target_state = target->paired_state;
      target_animation = &target_state->animation;
      if(target_animation->primary_state ==
         RIDER_ATTACK_BLOCKED_PRIMARY_STATE)
        {
          return;
        }

      attacks_right = is_rider_right_attack_animation(rider);
      attack_style = (RiderAttackStyle)state->attack_style;
      if(riders_overlap_for_attack_contact(
           rider, target_state, attacks_right, attack_style) == false)
        {
          return;
        }

      frame = frame_tick;
      if((unsigned int)target_state->attack_target_refresh_tick <
         (unsigned int)frame &&
         target_animation->primary_frame <= 1 &&
         attack_style == RIDER_ATTACK_STYLE_PUNCH &&
         target_state->attack_style != RIDER_ATTACK_STYLE_PUNCH &&
         target_state->attack_style != RIDER_ATTACK_STYLE_KICK &&
         is_rider_attack_animation(target) != false &&
         (rand() & RIDER_ATTACK_RANDOM_MASK) ==
         RIDER_ATTACK_TARGET_SWAP_ROLL)
        {
          state->queued_attack_style = target_state->queued_attack_style;
          state->attack_style = target_state->queued_attack_style;
          target_state->queued_attack_style = RIDER_ATTACK_STYLE_PUNCH;
          target_state->attack_style = RIDER_ATTACK_STYLE_PUNCH;
          state->attack_target_refresh_tick =
            frame + RIDER_ATTACK_TARGET_SWAP_DELAY_TICKS;

          attacks_left = is_rider_left_attack_animation(target);
          select_primary_rider_animation(
            animation,
            opponent_resource_group_c[
              attacks_left * RIDER_ATTACK_STYLE_COUNT +
              state->attack_style]);
          select_secondary_rider_animation(
            animation, RIDER_ATTACK_RIDE_ANIMATION);
          select_primary_rider_animation(
            target_animation, RIDER_ATTACK_RIDE_ANIMATION);
          select_secondary_rider_animation(
            target_animation, RIDER_ATTACK_RIDE_ANIMATION);
          return;
        }

      attacks_right = is_rider_right_attack_animation(rider);
      if((unsigned int)target_state->attack_contact_tick <
         (unsigned int)frame)
        {
          apply_rider_contact_damage(rider, target);
          accumulate_rider_contact_response(
            target, rider, RIDER_CONTACT_RESPONSE_TARGET_SLOT);
          accumulate_rider_contact_response(
            rider, target, RIDER_CONTACT_RESPONSE_SOURCE_SLOT);

          profile_level = active_rider_profile_level();
          random_roll = signed_divide_with_remainder(
            RIDER_ATTACK_DIFFICULTY_ROLL_BASE - profile_level,
            (unsigned int)rand());
          if(random_roll.remainder == 0)
            {
              select_secondary_rider_animation(
                animation, state->animation.primary_state);
            }
          else if(rider != player &&
                  (rand() & RIDER_ATTACK_RANDOM_MASK) ==
                  RIDER_ATTACK_ALTERNATE_ANIMATION_ROLL)
            {
              select_secondary_rider_animation(
                animation, RIDER_ATTACK_ONE_ARM_ANIMATION);
            }

          target_state->attack_contact_tick =
            frame + RIDER_ATTACK_CONTACT_BASE_DELAY_TICKS -
            profile_level * RIDER_ATTACK_CONTACT_LEVEL_DELAY_TICKS;

          if(is_rider_low_attack_animation(rider) != false)
            {
              target->velocity.forward -= RIDER_ATTACK_LONGITUDINAL_IMPULSE;
              rider->velocity.forward += RIDER_ATTACK_LONGITUDINAL_IMPULSE;
            }

          if(attacks_right == false)
            {
              target->steering_angle -=
                RIDER_ATTACK_TARGET_STEERING_IMPULSE;
              rider->steering_angle +=
                RIDER_ATTACK_SOURCE_STEERING_IMPULSE;
            }
          else
            {
              target->steering_angle +=
                RIDER_ATTACK_TARGET_STEERING_IMPULSE;
              rider->steering_angle -=
                RIDER_ATTACK_SOURCE_STEERING_IMPULSE;
            }
        }

      if(attack_style == RIDER_ATTACK_STYLE_KICK)
        {
          if(attacks_right == false)
            {
              target->steering_angle -= RIDER_ATTACK_KICK_STEERING_IMPULSE;
            }
          else
            {
              target->steering_angle += RIDER_ATTACK_KICK_STEERING_IMPULSE;
            }
        }

      clamp_rider_steering_angle(rider);
      clamp_rider_steering_angle(target);
      return;
    }

  if(contact_candidate == 0 ||
     contact_candidate->collision_class != RIDER_ATTACK_CONTACT_CLASS)
    {
      return;
    }

  attacks_right = is_rider_right_attack_animation(rider);
  attack_style = (RiderAttackStyle)state->attack_style;
  if(riders_overlap_for_attack_contact(
       rider, contact_candidate, attacks_right, attack_style) == false)
    {
      return;
    }

  contact_candidate->contact = rider;
  if(contact_candidate->contact_callback != 0)
    {
      contact_candidate->contact_callback(contact_candidate);
    }
}

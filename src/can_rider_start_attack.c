#include "stdlib.h"

#include "rider_contact_runtime.h"

enum RiderAttackEligibilityConstant
{
  RIDER_ATTACK_VELOCITY_PROJECTION_SHIFT = 4,
  RIDER_ATTACK_LONGITUDINAL_RADIUS_SHIFT = 4,
  RIDER_ATTACK_LATERAL_RADIUS_MULTIPLIER_SHIFT = 1,
  RIDER_ATTACK_LATERAL_RADIUS_SHIFT = 12,
  RIDER_ATTACK_RANDOM_MASK = 0xFF,
  RIDER_ATTACK_SCORE_COMPONENT = 1
};

boolean
can_rider_start_attack(RacerEntity *rider,
                       RacerEntity *target)
{
  RacerEntity *state;
  RacerEntity *target_state;
  RiderInteractionRuntime *interaction;
  RiderInteractionRuntime *target_interaction;
  RacerEntity *challenge;
  unsigned int current_tick;
  int distance;
  int radius;
  int random_value;
  int profile_index;
  int profile_level;
  signed char metric_index;

  state = rider->paired_state;
  current_tick = frame_tick;
  if(current_tick <= (unsigned int)state->attack_contact_tick)
    {
      return false;
    }

  target_state = target->paired_state;
  if(current_tick <= (unsigned int)target_state->attack_contact_tick)
    {
      return false;
    }

  distance = rider->velocity.forward - target_state->velocity.forward;
  distance = rider->position_z +
             (distance >> RIDER_ATTACK_VELOCITY_PROJECTION_SHIFT) -
             target_state->position_z;
  if(distance <= 0)
    {
      distance = -distance;
    }

  interaction = state->interaction_data;
  radius = interaction->engagement_radius;
  if(distance >=
     (radius << RIDER_ATTACK_LONGITUDINAL_RADIUS_SHIFT))
    {
      return false;
    }

  distance = rider->position_x - target_state->position_x;
  if(distance <= 0)
    {
      distance = -distance;
    }
  radius <<= RIDER_ATTACK_LATERAL_RADIUS_MULTIPLIER_SHIFT;
  if(distance >= (radius << RIDER_ATTACK_LATERAL_RADIUS_SHIFT))
    {
      return false;
    }

  random_value = rand() & RIDER_ATTACK_RANDOM_MASK;
  profile_index = race_mode_state.active_player_index;
  profile_level = player_profiles[profile_index].level;
  if(random_value >=
     rider_contact_delay_thresholds[profile_level] * rider->time_step)
    {
      return false;
    }

  challenge = (RacerEntity *)race_rider_state.challenge_object;
  if(challenge == 0)
    {
      return true;
    }
  if(state != challenge->paired_state)
    {
      return true;
    }

  if(state->attack_style != RIDER_ATTACK_STYLE_CLUB)
    {
      return false;
    }

  target_interaction = target_state->interaction_data;
  metric_index = target_interaction->metric_index;
  if(metric_index >= RIDER_INTERACTION_METRIC_COUNT)
    {
      return false;
    }

  return (boolean)(
    interaction->current_metrics[metric_index]
    .components[RIDER_ATTACK_SCORE_COMPONENT] >
    interaction->target_metrics[metric_index]
    .components[RIDER_ATTACK_SCORE_COMPONENT]);
}

#include "racer_runtime.h"

enum
{
  COLLISION_DAMAGE_NONE = 0,
  COLLISION_DAMAGE_STANDARD = 1,
  COLLISION_DAMAGE_SEVERE = 2,
  COLLISION_DAMAGE_GLANCING = 3,
  COLLISION_DAMAGE_ALTERNATE_ANIMATION_A = 4,
  COLLISION_DAMAGE_ALTERNATE_ANIMATION_B = 5,
  COLLISION_DAMAGE_FORCED_RECOVERY = 6,
  SEVERE_DAMAGE_MULTIPLIER = 2,
  GLANCING_DAMAGE_SHIFT = 3,
  GLANCING_DAMAGE_ROUNDING_BIAS = 7,
  ALTERNATE_STATE_FORWARD_VELOCITY = -0x50,
  DEFAULT_STATE_FORWARD_VELOCITY = -0x100,
  DEFAULT_STATE_VERTICAL_VELOCITY = 0x700,
  DEFAULT_RIDER_VERTICAL_VELOCITY = 0x400,
  ALTERNATE_PRIMARY_ANIMATION = 9,
  COLLISION_STEERING_FRACTION_BITS = 8,
  COLLISION_STEERING_ROUNDING_BIAS = 0xFF,
  RIDER_MODE_NORMAL = 0
};

int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);

static
int
divide_by_eight_toward_zero(int value)
{
  if(value < 0)
    {
      value += GLANCING_DAMAGE_ROUNDING_BIAS;
    }
  return value >> GLANCING_DAMAGE_SHIFT;
}


int
apply_rider_collision_damage(RacerEntity *rider,
                             int          rider_damage,
                             int          bike_damage,
                             int          recovery_kind)
{
  RacerVelocity rider_velocity_delta;
  RacerVelocity state_velocity_delta;
  RacerEntity *state;
  int remaining_rider_health;
  int remaining_bike_health;
  int steering_delta;
  int steering_value;
  int steering_limit;
  int recovery_started;

  state = rider->paired_state;

  switch(recovery_kind)
    {
    case COLLISION_DAMAGE_NONE:
    case COLLISION_DAMAGE_FORCED_RECOVERY:
      rider_damage = 0;
      bike_damage = 0;
      break;
    case COLLISION_DAMAGE_STANDARD:
    case COLLISION_DAMAGE_ALTERNATE_ANIMATION_A:
    case COLLISION_DAMAGE_ALTERNATE_ANIMATION_B:
      break;
    case COLLISION_DAMAGE_SEVERE:
      rider_damage *= SEVERE_DAMAGE_MULTIPLIER;
      bike_damage *= SEVERE_DAMAGE_MULTIPLIER;
      break;
    case COLLISION_DAMAGE_GLANCING:
      rider_damage = divide_by_eight_toward_zero(rider_damage);
      bike_damage = divide_by_eight_toward_zero(bike_damage);
      break;
    default:
      rider_damage = 0;
      bike_damage = 0;
      break;
    }

  remaining_rider_health = apply_rider_health_damage(rider, rider_damage);
  remaining_bike_health = apply_bike_health_damage(rider, bike_damage);
  recovery_started = 0;

  if(remaining_bike_health == 0 || remaining_rider_health == 0)
    {
      state_velocity_delta.lateral = 0;
      if(recovery_kind == COLLISION_DAMAGE_ALTERNATE_ANIMATION_A)
        {
          state_velocity_delta.forward =
            ALTERNATE_STATE_FORWARD_VELOCITY;
          state_velocity_delta.vertical = 0;
          rider_velocity_delta.lateral = 0;
          rider_velocity_delta.vertical = 0;
          rider_velocity_delta.forward = 0;
        }
      else
        {
          state_velocity_delta.vertical =
            DEFAULT_STATE_VERTICAL_VELOCITY;
          state_velocity_delta.forward =
            DEFAULT_STATE_FORWARD_VELOCITY;
          rider_velocity_delta.vertical =
            DEFAULT_RIDER_VERTICAL_VELOCITY;
          rider_velocity_delta.lateral = 0;
          rider_velocity_delta.forward = 0;
        }
      begin_rider_recovery(rider, &state_velocity_delta,
                           &rider_velocity_delta, recovery_kind);
    }

  if(recovery_kind == COLLISION_DAMAGE_ALTERNATE_ANIMATION_A)
    {
      if(rider->rider_mode == RIDER_MODE_NORMAL)
        {
          select_primary_rider_animation(
            &state->animation, ALTERNATE_PRIMARY_ANIMATION);
        }
    }
  else if(recovery_kind == COLLISION_DAMAGE_ALTERNATE_ANIMATION_B)
    {
      rider->contact_impulse.lateral =
        rider_damage * rider->contact_impulse.lateral;
    }

  steering_delta = rider->contact_impulse.lateral;
  if(steering_delta != 0 && rider->rider_mode == RIDER_MODE_NORMAL)
    {
      steering_delta =
        rider->collision_steering_gain_8_8 * steering_delta;
      if(steering_delta < 0)
        {
          steering_delta += COLLISION_STEERING_ROUNDING_BIAS;
        }
      steering_value = rider->steering_angle +
                       (steering_delta >>
                        COLLISION_STEERING_FRACTION_BITS);
      rider->steering_angle = steering_value;

      steering_limit = rider->steering_limit;
      if(steering_value > steering_limit)
        {
          rider->steering_angle = steering_limit;
        }
      else if(steering_value < -steering_limit)
        {
          rider->steering_angle = -steering_limit;
        }
    }
  else if(rider->rider_mode != RIDER_MODE_NORMAL)
    {
      recovery_started = 1;
    }

  return recovery_started;
}

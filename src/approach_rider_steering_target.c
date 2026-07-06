#include "division_helpers.h"
#include "racer_runtime.h"
#include "rider_physics_configuration.h"

enum RiderSteeringControlConstant
{
  RIDER_STEERING_ANGLE_LIMIT = 0x200000,
  RIDER_STEERING_FIXED_SHIFT = 8,
  RIDER_STEERING_FIXED_ROUNDING_BIAS = 0xFF,
  RIDER_STEERING_LIMIT_TURN_SHIFT = 2,
  RIDER_STEERING_LIMIT_TURN_ROUNDING_BIAS = 3
};

int
fixed_sine(int angle);

static
int
truncate_steering_fixed(int value)
{
  if(value < 0)
    {
      value += RIDER_STEERING_FIXED_ROUNDING_BIAS;
    }
  return value >> RIDER_STEERING_FIXED_SHIFT;
}


static
int
quarter_toward_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_STEERING_LIMIT_TURN_ROUNDING_BIAS;
    }
  return value >> RIDER_STEERING_LIMIT_TURN_SHIFT;
}


int
approach_rider_steering_target(RacerEntity *rider,
                               int          target_angle,
                               int          turn_direction)
{
  RiderPhysicsConfiguration *configuration;
  SignedDivisionResult division;
  int speed;
  int target;
  int target_magnitude;
  int limit;
  int current;
  int current_magnitude;
  int response_step;
  int maximum_step;
  int delta;
  int scaled_sine;
  int steering_velocity_scale;

  configuration = (RiderPhysicsConfiguration *)rider;
  speed = rider->velocity.forward;
  if(speed <= 0)
    {
      rider->steering_force = 0;
      rider->velocity.lateral = 0;
      return speed;
    }

  if(target_angle > RIDER_STEERING_ANGLE_LIMIT)
    {
      target_angle = RIDER_STEERING_ANGLE_LIMIT;
    }
  else if(target_angle < -RIDER_STEERING_ANGLE_LIMIT)
    {
      target_angle = -RIDER_STEERING_ANGLE_LIMIT;
    }

  scaled_sine = truncate_steering_fixed(fixed_sine(target_angle));
  steering_velocity_scale = configuration->steering_velocity_scale;
  division = signed_divide_with_remainder(
    (unsigned int)steering_velocity_scale,
    (unsigned int)(speed * scaled_sine));
  target = division.quotient;

  if(turn_direction == 0)
    {
      limit = 0;
    }
  else
    {
      limit = quarter_toward_zero(configuration->steering_limit);
    }
  limit += configuration->steering_limit;

  if(target > limit)
    {
      target = limit;
    }
  else if(target < -limit)
    {
      target = -limit;
    }

  current = configuration->steering_angle;
  configuration->previous_steering_angle = current;

  current_magnitude = current < 0 ? -current : current;
  target_magnitude = target < 0 ? -target : target;
  if(target != 0 && current_magnitude < target_magnitude)
    {
      response_step = configuration->steering_acceleration_step;
    }
  else
    {
      response_step = configuration->steering_deceleration_step;
    }
  maximum_step = rider->time_step * response_step;

  if(current > target)
    {
      delta = current - target;
      if(delta <= maximum_step)
        {
          current = target;
        }
      else if(turn_direction < 0)
        {
          current -= truncate_steering_fixed(
            configuration->left_steering_rate_scale_8_8 *
            maximum_step);
        }
      else if(turn_direction == 0)
        {
          current -= maximum_step;
        }
      else
        {
          current -= truncate_steering_fixed(
            configuration->right_steering_rate_scale_8_8 *
            maximum_step);
        }
    }
  else if(current < target)
    {
      delta = target - current;
      if(delta <= maximum_step)
        {
          current = target;
        }
      else if(turn_direction > 0)
        {
          current += truncate_steering_fixed(
            configuration->left_steering_rate_scale_8_8 *
            maximum_step);
        }
      else if(turn_direction == 0)
        {
          current += maximum_step;
        }
      else
        {
          current += truncate_steering_fixed(
            configuration->right_steering_rate_scale_8_8 *
            maximum_step);
        }
    }

  configuration->steering_angle = current;
  rider->velocity.lateral =
    steering_velocity_scale * truncate_steering_fixed(current);
  return rider->velocity.lateral;
}

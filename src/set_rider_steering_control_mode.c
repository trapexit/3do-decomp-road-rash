#include "racer_runtime.h"
#include "rider_physics_configuration.h"
#include "rw_semantic_data.h"

enum RiderControlConstant
{
  RIDER_CONTROL_MODE_LEFT = 1,
  RIDER_CONTROL_MODE_RIGHT = 2,
  RIDER_STEERING_MODE_ANGLE = 0x200000,
  RIDER_STEERING_MODE_DIRECTION_SHIFT = 21,
  RIDER_STATUS_CONTROL_DISABLED = 0x10,
  RIDER_STATUS_BOOST_ACTIVE = 0x20,
  RIDER_FIXED_8_SHIFT = 8,
  RIDER_FIXED_8_ROUNDING_BIAS = 0xFF,
  RIDER_FIXED_16_SHIFT = 16,
  RIDER_FIXED_16_ROUNDING_BIAS = 0xFFFF,
  RIDER_DISABLED_BRAKING_MULTIPLIER = 3,
  RIDER_DISABLED_BRAKING_SHIFT = 7
};

static
int
truncate_fixed_8(int value)
{
  if(value < 0)
    {
      value += RIDER_FIXED_8_ROUNDING_BIAS;
    }
  return value >> RIDER_FIXED_8_SHIFT;
}


static
int
truncate_fixed_16(int value)
{
  if(value < 0)
    {
      value += RIDER_FIXED_16_ROUNDING_BIAS;
    }
  return value >> RIDER_FIXED_16_SHIFT;
}


int
set_rider_steering_control_mode(RacerEntity *rider,
                                int          mode,
                                int          turn_direction)
{
  int target_angle;

  if(mode == RIDER_CONTROL_MODE_LEFT)
    {
      return approach_rider_steering_target(
        rider, -RIDER_STEERING_MODE_ANGLE, turn_direction);
    }
  if(mode == RIDER_CONTROL_MODE_RIGHT)
    {
      return approach_rider_steering_target(
        rider, RIDER_STEERING_MODE_ANGLE, turn_direction);
    }

  if(turn_direction == 0)
    {
      target_angle = 0;
    }
  else
    {
      target_angle =
        (int)((unsigned int)turn_direction <<
              RIDER_STEERING_MODE_DIRECTION_SHIFT);
      turn_direction = -turn_direction;
    }
  return approach_rider_steering_target(
    rider, target_angle, turn_direction);
}


int
approach_rider_acceleration_target(RacerEntity *rider,
                                   int          target)
{
  RiderPhysicsConfiguration *configuration;
  RiderTransmissionRuntime *transmission;
  RiderGearProfile *gear;
  int current;
  int step;

  configuration = (RiderPhysicsConfiguration *)rider;
  transmission = (RiderTransmissionRuntime *)rider;

  if((rider->status_flags & RIDER_STATUS_CONTROL_DISABLED) != 0 ||
     (rider == (RacerEntity *)race_rider_state.player_object &&
      input_thread_state.deferred_event_ticks > 0))
    {
      return approach_rider_braking_target(
        rider, configuration->maximum_reverse_control);
    }

  if(target > configuration->maximum_forward_control)
    {
      target = configuration->maximum_forward_control;
    }

  current = configuration->longitudinal_control;
  if(current < target)
    {
      step = rider->time_step *
             configuration->forward_acceleration_step;
      if(target - current <= step)
        {
          current = target;
        }
      else
        {
          current += step;
        }
    }
  else if(current > target)
    {
      step = rider->time_step *
             configuration->forward_deceleration_step;
      if(current - target <= step)
        {
          current = target;
        }
      else
        {
          current -= step;
        }
    }

  if(configuration->drive_disabled == 0)
    {
      if(current > 0)
        {
          gear = &transmission->profiles[transmission->current_gear];
          rider->drive_acceleration = truncate_fixed_16(
            current * gear->maximum_forward_speed);
          if((rider->status_flags & RIDER_STATUS_BOOST_ACTIVE) != 0)
            {
              rider->drive_acceleration =
                (race_boost_config.active_speed_scale_8_8 *
                 rider->drive_acceleration) >> RIDER_FIXED_8_SHIFT;
            }
        }
      else
        {
          rider->drive_acceleration = truncate_fixed_8(current);
        }
    }
  else
    {
      rider->drive_acceleration = 0;
    }

  configuration->longitudinal_control = current;
  return (int)&configuration->longitudinal_control;
}


int
approach_rider_braking_target(RacerEntity *rider,
                              int          target)
{
  RiderPhysicsConfiguration *configuration;
  int current;
  int step;

  configuration = (RiderPhysicsConfiguration *)rider;
  current = configuration->longitudinal_control;

  if((rider->status_flags & RIDER_STATUS_CONTROL_DISABLED) != 0)
    {
      target = configuration->maximum_reverse_control;
      target = target * RIDER_DISABLED_BRAKING_MULTIPLIER;
      target <<= RIDER_DISABLED_BRAKING_SHIFT;
      target = truncate_fixed_8(target);
    }
  else if(target < configuration->maximum_reverse_control)
    {
      target = configuration->maximum_reverse_control;
    }

  if(current < target)
    {
      step = rider->time_step * configuration->reverse_deceleration_step;
      if(current - target <= step)
        {
          current = target;
        }
      else
        {
          current -= step;
        }
    }
  else if(current > target)
    {
      step = rider->time_step * configuration->reverse_acceleration_step;
      if(current <= step)
        {
          current = target;
        }
      else
        {
          current += step;
        }
    }

  if(rider->velocity.forward < 0)
    {
      rider->drive_acceleration = truncate_fixed_8(-current);
    }
  else
    {
      rider->drive_acceleration = truncate_fixed_8(current);
    }
  configuration->longitudinal_control = current;
  return (int)&configuration->longitudinal_control;
}

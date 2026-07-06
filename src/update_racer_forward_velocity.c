#include "racer_runtime.h"

#include "assert.h"

enum RacerForwardVelocityConstant
{
  RACER_FORWARD_SURFACE_DRAG_SHIFT = 14,
  RACER_FORWARD_SURFACE_DRAG_ROUNDING_BIAS = 0x3FFF,
  RACER_FORWARD_SPEED_DRAG_SHIFT = 16,
  RACER_FORWARD_SPEED_DRAG_ROUNDING_BIAS = 0xFFFF,
  RACER_FORWARD_NO_CONTACT_SPEED_SHIFT = 2,
  RACER_FORWARD_STEERING_LOSS_SHIFT = 4,
  RACER_FORWARD_STEERING_LOSS_ROUNDING_BIAS = 0x0F,
  RACER_FORWARD_MINIMUM_DRIVE_SPEED = 100
};

__inline
static
int
absolute_nonpositive_value(int value)
{
  if(value <= 0)
    {
      return -value;
    }
  return value;
}


__inline
static
int
divide_power_of_two_toward_zero(int value,
                                int shift,
                                int rounding_bias)
{
  if(value < 0)
    {
      value += rounding_bias;
    }
  return value >> shift;
}


void
update_racer_forward_velocity(RacerEntity *racer_,
                              int          surface_zone_)
{
  RacerEntity *racer = racer_;
  int speed_magnitude;
  int surface_drag;
  int combined_drag;
  int steering_loss;
  int acceleration;
  int speed;

  assert(racer != NULL);

  speed_magnitude = absolute_nonpositive_value(
    racer->velocity.forward);
  if(racer->surface_contact_scale_8_8 != 0)
    {
      assert((surface_zone_ >= 0) && (surface_zone_ < RIDER_PHYSICS_SURFACE_COUNT));
      surface_drag = rider_physics_tuning.surface_drag[surface_zone_];
      racer->surface_drag = surface_drag;

      combined_drag = racer->base_drag +
                      divide_power_of_two_toward_zero(
        speed_magnitude * surface_drag,
        RACER_FORWARD_SURFACE_DRAG_SHIFT,
        RACER_FORWARD_SURFACE_DRAG_ROUNDING_BIAS);
    }
  else
    {
      speed_magnitude >>= RACER_FORWARD_NO_CONTACT_SPEED_SHIFT;
      combined_drag = 0;
    }

  combined_drag += divide_power_of_two_toward_zero(
    speed_magnitude * racer->reserved_150,
    RACER_FORWARD_SPEED_DRAG_SHIFT,
    RACER_FORWARD_SPEED_DRAG_ROUNDING_BIAS);
  racer->drag_limit = combined_drag;

  steering_loss = divide_power_of_two_toward_zero(
    absolute_nonpositive_value(racer->steering_force),
    RACER_FORWARD_STEERING_LOSS_SHIFT,
    RACER_FORWARD_STEERING_LOSS_ROUNDING_BIAS);

  speed = racer->velocity.forward;
  if(speed >= 0)
    {
      acceleration = racer->drive_acceleration -
                     combined_drag - steering_loss;
      speed += racer->time_step * acceleration;
      if(speed > 0)
        {
          racer->velocity.forward = speed;
        }
      else if(racer->drive_acceleration > 0)
        {
          racer->velocity.forward =
            RACER_FORWARD_MINIMUM_DRIVE_SPEED;
        }
      else
        {
          racer->velocity.forward = 0;
        }
    }
  else
    {
      acceleration = absolute_nonpositive_value(
        combined_drag + steering_loss);
      acceleration = racer->drive_acceleration + acceleration;
      speed += racer->time_step * acceleration;
      if(speed <= 0)
        {
          racer->velocity.forward = speed;
        }
      else
        {
          racer->velocity.forward = 0;
        }
    }

  rider_physics_tuning.acceleration_drag = acceleration;
}

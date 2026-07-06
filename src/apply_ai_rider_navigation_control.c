#include "ai_collision_avoidance_runtime.h"

enum AiRiderNavigationConstant
{
  AI_RIDER_FAST_SLIP_THRESHOLD = 0x28,
  AI_RIDER_FAST_STEERING_SCALE = 4,
  AI_RIDER_TIME_STEP_ROUNDING_BIAS = 7,
  AI_RIDER_TIME_STEP_SHIFT = 3,
  AI_RIDER_STEERING_CORRECTION_SHIFT = 2,
  AI_RIDER_LATERAL_FIXED_SHIFT = 8,
  AI_RIDER_ANGLE_SCALE_SHIFT = 3,
  AI_RIDER_ANGLE_MASK = 0x00FFFFFF,
  AI_RIDER_ANGLE_SIGN_LIMIT = 0x00800000,
  AI_RIDER_STEERING_HEADING_LIMIT = 0x0E,
  AI_RIDER_SPEED_PRIMARY_SHIFT = 7,
  AI_RIDER_SPEED_SECONDARY_SHIFT = 9,
  AI_RIDER_SPEED_TERTIARY_SHIFT = 12,
  AI_RIDER_HALF_SPEED_SIGN_SHIFT = 31,
  AI_RIDER_HALF_SPEED_SHIFT = 1,
  AI_RIDER_FIXED_ROUNDING_BIAS = 0xFF
};

#define AI_RIDER_ANGLE_SIGN_EXTENSION ((int)0xFF000000U)

int
fixed_vector_angle_24(int horizontal,
                      int vertical);

static
int
truncate_lateral_fixed(int value)
{
  if(value < 0)
    {
      value += AI_RIDER_FIXED_ROUNDING_BIAS;
    }
  return value >> AI_RIDER_LATERAL_FIXED_SHIFT;
}


static
int
half_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >>
                AI_RIDER_HALF_SPEED_SIGN_SHIFT)) >>
         AI_RIDER_HALF_SPEED_SHIFT;
}


static
int
rider_speed_units(int value)
{
  return (value >> AI_RIDER_SPEED_PRIMARY_SHIFT) +
         (value >> AI_RIDER_SPEED_SECONDARY_SHIFT) +
         (value >> AI_RIDER_SPEED_TERTIARY_SHIFT);
}


int
apply_ai_rider_navigation_control(RacerEntity *rider)
{
  RiderInteractionRuntime *interaction;
  int uses_fast_steering;
  int steering_scale;
  int time_step_units;
  int longitudinal_delta;
  int lateral_delta;
  int target_angle;
  int speed;
  int target_speed;
  int heading_magnitude;

  interaction = rider->paired_state->interaction_data;

  uses_fast_steering =
    rider->slip_amount > AI_RIDER_FAST_SLIP_THRESHOLD;
  steering_scale = uses_fast_steering
                         ? AI_RIDER_FAST_STEERING_SCALE
                         : 1;

  longitudinal_delta = interaction->target_track_position -
                       rider->position_z;
  lateral_delta = interaction->target_lateral_position -
                  rider->position_x;

  steering_scale *= rider->steering_angle;
  time_step_units = rider->time_step;
  if(time_step_units < 0)
    {
      time_step_units += AI_RIDER_TIME_STEP_ROUNDING_BIAS;
    }
  time_step_units =
    (time_step_units >> AI_RIDER_TIME_STEP_SHIFT) + 1;
  lateral_delta -=
    (time_step_units * steering_scale) <<
      AI_RIDER_STEERING_CORRECTION_SHIFT;

  target_angle = fixed_vector_angle_24(
    longitudinal_delta, truncate_lateral_fixed(lateral_delta));
  target_angle &= AI_RIDER_ANGLE_MASK;
  if(target_angle >= AI_RIDER_ANGLE_SIGN_LIMIT)
    {
      target_angle += AI_RIDER_ANGLE_SIGN_EXTENSION;
    }

  if(rider->surface_contact_scale_8_8 == 0)
    {
      heading_magnitude = rider->orientation.steering_heading;
      if(heading_magnitude <= 0)
        {
          heading_magnitude = -heading_magnitude;
        }
      if(heading_magnitude >= AI_RIDER_STEERING_HEADING_LIMIT)
        {
          approach_rider_steering_target(
            rider,
            target_angle << AI_RIDER_ANGLE_SCALE_SHIFT, 0);
        }
    }
  else
    {
      approach_rider_steering_target(
        rider,
        target_angle << AI_RIDER_ANGLE_SCALE_SHIFT, 0);
    }

  speed = rider_speed_units(rider->velocity.forward);
  target_speed = interaction->target_speed;

  if(speed < target_speed)
    {
      if(uses_fast_steering == 0 ||
         half_toward_zero(target_speed) > speed)
        {
          return approach_rider_acceleration_target(
            rider, rider->acceleration_limit);
        }
    }
  else if(speed > target_speed)
    {
      return approach_rider_braking_target(
        rider, rider->braking_target);
    }

  return 0;
}


#undef AI_RIDER_ANGLE_SIGN_EXTENSION

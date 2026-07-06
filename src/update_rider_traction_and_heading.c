#include "racer_runtime.h"

#include "assert.h"

enum RiderTractionConstant
{
  RIDER_TRACTION_SIGN_SHIFT = 31,
  RIDER_TRACTION_EIGHTH_SHIFT = 3,
  RIDER_TRACTION_FIXED_SHIFT = 8,
  RIDER_TRACTION_SPEED_SHIFT = 11,
  RIDER_TRACTION_SLIDE_TIMER_SHIFT = 12,
  RIDER_TRACTION_EIGHTH_ROUNDING = 7,
  RIDER_TRACTION_FIXED_ROUNDING = 0xFF,
  RIDER_TRACTION_SPEED_ROUNDING = 0x7FF,
  RIDER_TRACTION_ANGLE_HALF_TURN = 0x800000,
  RIDER_TRACTION_ANGLE_FULL_TURN = 0x1000000,
  RIDER_TRACTION_RECOVERY_DELAY = 0x19,
  RIDER_TRACTION_STATE_IMPULSE = -0x600,
  RIDER_TRACTION_RIDER_IMPULSE = 0x400,
  RIDER_TRACTION_RECOVERY_KIND = 6,
  RIDER_TRACTION_LANE_MASK = 7,
  RIDER_TRACTION_ROAD_SURFACE = 1,
  RIDER_TRACTION_SLIP_MASK = 0xFF,
  RIDER_TRACTION_SPEED_LOSS_THRESHOLD = 0x28,
  RIDER_TRACTION_SLIDING_FLAG = 4,
  RIDER_TRACTION_RIGHT_SLIP_FLAG = 8,
  RIDER_TRACTION_LEFT_SLIP_FLAG = 0x10,
  RIDER_TRACTION_LOW_SPEED = 4000,
  RIDER_TRACTION_HIGH_SPEED = 8000
};

int
fixed_vector_angle_24(int x,
                      int y);

static
int
divide_by_two_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> RIDER_TRACTION_SIGN_SHIFT)) >> 1;
}


static
int
divide_by_eight_towards_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_TRACTION_EIGHTH_ROUNDING;
    }
  return value >> RIDER_TRACTION_EIGHTH_SHIFT;
}


static
int
fixed_8_8_to_integer_towards_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_TRACTION_FIXED_ROUNDING;
    }
  return value >> RIDER_TRACTION_FIXED_SHIFT;
}


static
int
divide_speed_by_2048_towards_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_TRACTION_SPEED_ROUNDING;
    }
  return value >> RIDER_TRACTION_SPEED_SHIFT;
}


static
int
absolute_nonpositive(int value)
{
  if(value < 1)
    {
      return -value;
    }
  return value;
}


static
int
normalized_rider_heading(int steering)
{
  int heading;

  heading = fixed_vector_angle_24(
    rider_physics_tuning.steering_heading_reference,
    fixed_8_8_to_integer_towards_zero(steering));
  if(heading > RIDER_TRACTION_ANGLE_HALF_TURN)
    {
      heading -= RIDER_TRACTION_ANGLE_FULL_TURN;
    }
  return heading;
}


static
void
clear_rider_traction_motion(RacerEntity *rider)
{
  rider->orientation.steering_heading =
    normalized_rider_heading(rider->steering_angle);
  rider->drive_acceleration = 0;
  rider->steering_force = 0;
  rider->velocity.lateral = 0;
  rider->slip_amount = 0;
}


int
update_rider_traction_and_heading(RacerEntity *rider_)
{
  RacerEntity *rider = rider_;
  RacerVelocity state_velocity_delta;
  RacerVelocity rider_velocity_delta;
  int surface;
  int surface_grip;
  int scaled_slide_timer;
  int slip_budget;
  int lateral_acceleration;
  int steering;
  int forward_speed;
  int speed_steering_component;
  int absolute_speed_component;
  int absolute_lateral_acceleration;
  int required_grip;
  int steering_force;
  int steering_limit;
  int lane;
  int lane_grip;
  unsigned int slip_amount;
  int heading_input;

  assert(rider != NULL);
  assert(rider->track_segment != NULL);

  surface = (signed char)classify_rider_surface_zone(
    (char *)rider->track_segment, rider->position_x);
  assert((surface >= 0) && (surface < RIDER_PHYSICS_SURFACE_COUNT));
  surface_grip = rider_physics_tuning.base_grip[surface];

  if(rider->surface_contact_scale_8_8 == 0 || rider->rider_mode != 0)
    {
      clear_rider_traction_motion(rider);
      return surface;
    }

  rider->steering_force = 0;
  if(rider->velocity.forward <= 0)
    {
      rider->steering_angle = 0;
    }

  rider->slide_timer -= rider->time_step;
  if(rider->slide_timer < 0)
    {
      rider->slide_timer = 0;
    }

  scaled_slide_timer =
    rider->slide_timer << RIDER_TRACTION_SLIDE_TIMER_SHIFT;
  if(rider->slip_amount != 0)
    {
      slip_budget = rider->active_slip_budget -
                    fixed_8_8_to_integer_towards_zero(scaled_slide_timer);
    }
  else
    {
      slip_budget = rider->base_slip_budget -
                    fixed_8_8_to_integer_towards_zero(scaled_slide_timer);
    }

  slip_budget = fixed_8_8_to_integer_towards_zero(
    rider->slide_grip_scale_8_8 * slip_budget);
  slip_budget = fixed_8_8_to_integer_towards_zero(
    surface_grip * slip_budget);
  slip_budget = fixed_8_8_to_integer_towards_zero(
    slip_budget * rider->surface_contact_scale_8_8);
  if(slip_budget < 0)
    {
      slip_budget = 0;
    }

  if(rider == (RacerEntity *)race_rider_state.player_object)
    {
      if(race_input_latch_state.event_latched == 0 &&
         slip_budget < rider->slide_activation_threshold &&
         absolute_nonpositive(rider->steering_angle) >=
         rider->steering_limit)
        {
          rider->action_flags = 1;
          rider_physics_tuning.player_slip_ticks += rider->time_step;
          if(rider_physics_tuning.player_slip_ticks >
             RIDER_TRACTION_RECOVERY_DELAY)
            {
              state_velocity_delta.forward =
                RIDER_TRACTION_STATE_IMPULSE;
              state_velocity_delta.lateral = 0;
              state_velocity_delta.vertical = 0;
              rider_velocity_delta.forward =
                RIDER_TRACTION_RIDER_IMPULSE;
              rider_velocity_delta.lateral = 0;
              rider_velocity_delta.vertical = 0;
              begin_rider_recovery(
                rider, &state_velocity_delta, &rider_velocity_delta,
                RIDER_TRACTION_RECOVERY_KIND);
            }
        }
      else
        {
          rider->action_flags = 0;
          rider_physics_tuning.player_slip_ticks = 0;
        }
    }
  else
    {
      rider->action_flags = 0;
    }

  lateral_acceleration = rider->drive_acceleration;
  steering_limit = rider->drag_limit;
  if(lateral_acceleration > steering_limit)
    {
      lateral_acceleration -= steering_limit;
    }
  else if(lateral_acceleration > 0)
    {
      lateral_acceleration = 0;
    }
  else if(rider->velocity.forward <= 0)
    {
      lateral_acceleration = 0;
    }

  steering = rider->steering_angle;
  steering_force = steering;
  forward_speed = rider->velocity.forward;
  speed_steering_component =
    divide_speed_by_2048_towards_zero(forward_speed) *
    (steering >> RIDER_TRACTION_FIXED_SHIFT);
  absolute_speed_component =
    absolute_nonpositive(speed_steering_component);
  absolute_lateral_acceleration =
    absolute_nonpositive(lateral_acceleration);
  required_grip =
    absolute_speed_component + absolute_lateral_acceleration;

  if(slip_budget < required_grip && forward_speed >= 0)
    {
      if(surface > RIDER_TRACTION_ROAD_SURFACE)
        {
          lane = (rider->position_z >> RIDER_TRACTION_FIXED_SHIFT) &
                 RIDER_TRACTION_LANE_MASK;
          lane_grip =
            rider_physics_tuning.lane_surface_grip[lane][surface];
          steering = fixed_8_8_to_integer_towards_zero(
            steering * lane_grip);
          rider->steering_angle = steering;

          steering_limit = rider->steering_limit;
          if(steering > steering_limit)
            {
              rider->steering_angle = steering_limit;
            }
          else if(steering < -steering_limit)
            {
              rider->steering_angle = -steering_limit;
            }
        }

      steering = rider->steering_angle;
      steering_force = fixed_8_8_to_integer_towards_zero(
        (steering - rider->previous_steering_angle) *
        rider_physics_tuning.steering_correction[surface]);
      steering_force += fixed_8_8_to_integer_towards_zero(
        steering * rider_physics_tuning.steering_gain[surface]);

      slip_amount = (unsigned int)(required_grip - slip_budget);
      rider->slip_amount = (unsigned char)slip_amount;
      if((slip_amount & RIDER_TRACTION_SLIP_MASK) >
         RIDER_TRACTION_SPEED_LOSS_THRESHOLD)
        {
          rider->velocity.forward -=
            rider_physics_tuning.skid_speed_loss[surface];
          if(rider->velocity.forward < 0)
            {
              rider->velocity.forward = 0;
            }
        }

      rider->action_flags = (unsigned char)(
        rider->action_flags | RIDER_TRACTION_SLIDING_FLAG);
      if(slip_budget < absolute_lateral_acceleration)
        {
          if(lateral_acceleration <= 0)
            {
              rider->action_flags = (unsigned char)(
                rider->action_flags | RIDER_TRACTION_LEFT_SLIP_FLAG);
            }
          else
            {
              rider->action_flags = (unsigned char)(
                rider->action_flags | RIDER_TRACTION_RIGHT_SLIP_FLAG);
            }
        }
    }
  else
    {
      rider->slip_amount = 0;
    }

  if(lateral_acceleration < 0)
    {
      steering_force = fixed_8_8_to_integer_towards_zero(
        rider->steering_speed_scale * steering_force);
    }
  rider->steering_force +=
    steering_force >> RIDER_TRACTION_FIXED_SHIFT;

  heading_input = rider->steering_angle;
  forward_speed = rider->velocity.forward;
  if(forward_speed < RIDER_TRACTION_LOW_SPEED)
    {
      heading_input = divide_by_two_towards_zero(heading_input);
    }
  else if(forward_speed > RIDER_TRACTION_HIGH_SPEED)
    {
      heading_input += divide_by_eight_towards_zero(heading_input);
    }

  rider->orientation.steering_heading =
    normalized_rider_heading(heading_input);
  return surface;
}

#include "racer_runtime.h"
#include "rider_timestep.h"

enum RacerMotionIntegrationConstant
{
  RACER_MOTION_SIGN_SHIFT = 31,
  RACER_MOTION_FIXED_FRACTION_BITS = 8,
  RACER_MOTION_FIXED_ROUNDING_BIAS = 0xFF,
  RACER_MOTION_FORWARD_ANGLE_SCALE = 4,
  RACER_MOTION_VELOCITY_DOUBLE = 2,
  RACER_MOTION_ALTERNATE_SURFACE_FLAG = 4,
  RACER_MOTION_LOCKED_HEADING_FLAG = 1,
  RACER_MOTION_HALF_TURN = 0x800000,
  RACER_MOTION_FULL_TURN = 0x1000000,
  RACER_MOTION_LOCKED_HEADING = 0x3F0000,
  RACER_MOTION_LOW_SPEED_THRESHOLD = 0xFA0,
  RACER_MOTION_LOW_SPEED_HEADING_LIMIT = 0x180000,
  RACER_MOTION_SURFACE_SLIP_THRESHOLD = 0x32,
  RACER_MOTION_PLAYER_SURFACE_SCALE = 800,
  RACER_MOTION_OTHER_SURFACE_SCALE = 600,
  RACER_MOTION_NORMAL_SURFACE_SCALE = 400,
  RACER_MOTION_POSITION_SPEED_THRESHOLD = 0x12C
};

int
fixed_vector_angle_24(int horizontal,
                      int vertical);
void
advance_racer_track_position(RacerEntity *racer,
                             int          distance,
                             int          update_track);

static
int
divide_by_two_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> RACER_MOTION_SIGN_SHIFT)) >> 1;
}


static
int
divide_by_256_towards_zero(int value)
{
  if(value < 0)
    {
      value += RACER_MOTION_FIXED_ROUNDING_BIAS;
    }
  return value >> RACER_MOTION_FIXED_FRACTION_BITS;
}


static
void
copy_current_to_previous_position(RacerEntity *racer)
{
  racer->previous_position_z = racer->position_z;
  racer->previous_position_y = racer->position_y;
  racer->previous_position_aux = racer->position_aux;
  racer->previous_position_x = racer->position_x;
  racer->previous_track_segment = racer->track_segment;
}


static
int
surface_acceleration_scale(const RacerEntity *racer,
                           unsigned int       action_flags)
{
  if((action_flags & RACER_MOTION_ALTERNATE_SURFACE_FLAG) != 0 &&
     racer->slip_amount > RACER_MOTION_SURFACE_SLIP_THRESHOLD)
    {
      if(racer == (RacerEntity *)race_rider_state.player_object &&
         race_input_latch_state.event_latched == 0)
        {
          return RACER_MOTION_PLAYER_SURFACE_SCALE;
        }
      return RACER_MOTION_OTHER_SURFACE_SCALE;
    }
  return RACER_MOTION_NORMAL_SURFACE_SCALE;
}


void
integrate_racer_motion_step(RacerEntity *racer)
{
  unsigned int action_flags;
  int motion_heading;
  int heading_scale;
  int acceleration;
  int surface_acceleration;
  int surface_scale;
  int surface_value;
  int heading;

  racer->world_velocity.forward = racer->velocity.forward;
  if(racer->surface_contact_scale_8_8 > 0)
    {
      racer->lateral_velocity_sample = racer->velocity.lateral;
      racer->world_velocity.vertical = 0;
      racer->world_velocity.lateral = racer->velocity.lateral;
    }
  else
    {
      racer->world_velocity.vertical = 0;
      racer->world_velocity.lateral = racer->lateral_velocity_sample;
    }

  if(racer->world_velocity.forward < 0)
    {
      motion_heading = fixed_vector_angle_24(
        -(racer->world_velocity.forward *
          RACER_MOTION_FORWARD_ANGLE_SCALE),
        -racer->world_velocity.lateral);
    }
  else
    {
      motion_heading = fixed_vector_angle_24(
        racer->world_velocity.forward *
        RACER_MOTION_FORWARD_ANGLE_SCALE,
        racer->world_velocity.lateral);
    }
  if(motion_heading > RACER_MOTION_HALF_TURN)
    {
      motion_heading -= RACER_MOTION_FULL_TURN;
    }

  action_flags = racer->action_flags;
  if((action_flags & RACER_MOTION_ALTERNATE_SURFACE_FLAG) != 0)
    {
      heading_scale = divide_by_256_towards_zero(
        racer->alternate_surface_heading_scale_8_8);
    }
  else
    {
      heading_scale = divide_by_256_towards_zero(
        racer->surface_heading_scale_8_8);
    }
  motion_heading *= heading_scale;

  if((action_flags & RACER_MOTION_LOCKED_HEADING_FLAG) != 0)
    {
      motion_heading = motion_heading <= 0
                             ? -RACER_MOTION_LOCKED_HEADING
                             : RACER_MOTION_LOCKED_HEADING;
    }
  if(racer->velocity.forward < RACER_MOTION_LOW_SPEED_THRESHOLD)
    {
      if(motion_heading > RACER_MOTION_LOW_SPEED_HEADING_LIMIT)
        {
          motion_heading = RACER_MOTION_LOW_SPEED_HEADING_LIMIT;
        }
      else if(motion_heading <
              -RACER_MOTION_LOW_SPEED_HEADING_LIMIT)
        {
          motion_heading = -RACER_MOTION_LOW_SPEED_HEADING_LIMIT;
        }
    }
  racer->orientation.movement_heading = motion_heading;

  acceleration = racer->world_velocity.forward *
                 RACER_MOTION_VELOCITY_DOUBLE -
                 racer->time_step * rider_physics_tuning.acceleration_drag;
  acceleration = divide_by_two_towards_zero(
    racer->time_step * acceleration);

  copy_current_to_previous_position(racer);
  advance_racer_track_position(
    racer, divide_by_256_towards_zero(acceleration), 1);

  heading = 0;
  surface_value = racer->right_surface_bound;
  if(surface_value != 0)
    {
      surface_scale = surface_acceleration_scale(racer, action_flags);
      surface_acceleration = _rider_timestep_quotient(racer->time_step,
                                                     (surface_value * surface_scale));
      surface_acceleration = divide_by_256_towards_zero(surface_acceleration);
      racer->world_velocity.lateral += surface_acceleration;

      heading = fixed_vector_angle_24(
        acceleration,
        divide_by_256_towards_zero(
          surface_value * RACER_MOTION_NORMAL_SURFACE_SCALE));
    }

  if(heading >= RACER_MOTION_HALF_TURN)
    {
      heading -= RACER_MOTION_FULL_TURN;
    }
  racer->heading = heading;

  if(racer->velocity.forward >
     RACER_MOTION_POSITION_SPEED_THRESHOLD)
    {
      racer->position_x +=
        racer->time_step * racer->world_velocity.lateral;
    }
}

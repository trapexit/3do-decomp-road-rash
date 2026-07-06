#include "racer_runtime.h"

enum TrackObjectMotionConstant
{
  TRACK_OBJECT_MOTION_SIGN_SHIFT = 31,
  TRACK_OBJECT_MOTION_HALF_SHIFT = 1,
  TRACK_OBJECT_MOTION_FIXED_SHIFT = 8,
  TRACK_OBJECT_MOTION_FIXED_ROUNDING_BIAS = 0xFF,
  TRACK_OBJECT_MOTION_ANGLE_MASK = 0x00FFFFFF
};

int
fixed_cosine(int angle);
int
fixed_sine(int angle);
void
advance_racer_track_position(RacerEntity *racer,
                             int          track_delta,
                             int          update_lane_bounds);

__inline
static
int
divide_by_two_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> TRACK_OBJECT_MOTION_SIGN_SHIFT)) >>
         TRACK_OBJECT_MOTION_HALF_SHIFT;
}


__inline
static
int
divide_fixed_8_8_toward_zero(int value)
{
  if(value < 0)
    {
      value += TRACK_OBJECT_MOTION_FIXED_ROUNDING_BIAS;
    }
  return value >> TRACK_OBJECT_MOTION_FIXED_SHIFT;
}


__inline
static
void
apply_pending_contact_impulse(RacerEntity *object)
{
  if(object->collision_flags != 0)
    {
      object->velocity.lateral +=
        divide_by_two_toward_zero(object->contact_impulse.lateral);
      object->velocity.vertical +=
        divide_by_two_toward_zero(object->contact_impulse.vertical);
      object->velocity.forward +=
        divide_by_two_toward_zero(object->contact_impulse.forward);

      object->contact_impulse.forward = 0;
      object->contact_impulse.lateral = 0;
      object->contact_impulse.vertical = 0;
    }
}


__inline
static
void
project_forward_velocity(RacerEntity *object)
{
  int angle;
  int direction;

  angle = object->heading;
  if(angle == 0)
    {
      object->world_velocity.lateral = 0;
      object->world_velocity.forward = object->velocity.forward;
      return;
    }

  angle &= TRACK_OBJECT_MOTION_ANGLE_MASK;
  direction = divide_fixed_8_8_toward_zero(fixed_cosine(angle));
  object->world_velocity.forward = divide_fixed_8_8_toward_zero(
    object->velocity.forward * direction);

  direction = divide_fixed_8_8_toward_zero(fixed_sine(angle));
  object->world_velocity.lateral = divide_fixed_8_8_toward_zero(
    object->velocity.forward * direction);
}


__inline
static
void
snapshot_track_position(RacerEntity *object)
{
  object->previous_position_z = object->position_z;
  object->previous_position_y = object->position_y;
  object->previous_position_aux = object->position_aux;
  object->previous_position_x = object->position_x;
  object->previous_track_segment = object->track_segment;
}


void
integrate_track_object_motion(RacerEntity *object)
{
  int drag;
  int track_velocity;
  int track_delta;

  apply_pending_contact_impulse(object);
  project_forward_velocity(object);
  snapshot_track_position(object);

  drag = divide_by_two_toward_zero(
    object->time_step * rider_physics_tuning.acceleration_drag);
  track_velocity = object->world_velocity.forward - drag;
  track_delta = divide_fixed_8_8_toward_zero(
    object->time_step * track_velocity);
  advance_racer_track_position(object, track_delta, 1);

  object->position_x +=
    object->time_step * object->world_velocity.lateral;
}

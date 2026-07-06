#include "racer_runtime.h"

int *
apply_race_object_heading_and_force(RacerEntity *object,
                                    int          heading,
                                    int          maximum_forward_velocity,
                                    int          acceleration)
{
  int forward_velocity;

  if(heading >= RACE_OBJECT_ANGLE_HALF_TURN)
    {
      heading -= RACE_OBJECT_ANGLE_FULL_TURN;
    }
  object->heading = heading;

  forward_velocity =
    object->time_step * acceleration + object->velocity.forward;
  object->velocity.forward = forward_velocity;
  if(forward_velocity > maximum_forward_velocity)
    {
      object->velocity.forward = maximum_forward_velocity;
    }

  object->orientation.movement_heading = object->heading;
  return &object->orientation.movement_heading;
}

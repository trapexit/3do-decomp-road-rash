#include "track_object_runtime.h"

int
fixed_vector_angle_24(int horizontal,
                      int vertical);

int *
apply_track_object_steering(TrackObject              *object,
                            const TrackMotionCommand *command,
                            int                       maximum_forward_velocity,
                            int                       acceleration)
{
  int direction_heading;

  direction_heading = fixed_vector_angle_24(
    command->direction,
    command->lateral_offset >> TRACK_OBJECT_POSITION_FRACTION_BITS);
  if(direction_heading >= RACE_OBJECT_ANGLE_HALF_TURN)
    {
      direction_heading -= RACE_OBJECT_ANGLE_FULL_TURN;
    }
  return apply_race_object_heading_and_force(
    (RacerEntity *)(void *)object, direction_heading,
    maximum_forward_velocity, acceleration);
}

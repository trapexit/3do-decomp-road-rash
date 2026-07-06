#include "track_object_runtime.h"

enum TrackObjectSpeedApproachConstant
{
  TRACK_OBJECT_SPEED_DISTANCE_RESPONSE_SHIFT = 3,
  TRACK_OBJECT_SPEED_DISTANCE_RESPONSE_UNIT =
    1 << TRACK_OBJECT_SPEED_DISTANCE_RESPONSE_SHIFT
};

static
int
scale_speed_distance_towards_zero(int distance)
{
  if(distance < 0)
    {
      distance += TRACK_OBJECT_SPEED_DISTANCE_RESPONSE_UNIT - 1;
    }
  return distance >> TRACK_OBJECT_SPEED_DISTANCE_RESPONSE_SHIFT;
}


int
approach_track_object_speed(const TrackObjectPosition *current_position,
                            int                        current_speed,
                            int                        maximum_speed,
                            int                        minimum_speed,
                            const TrackObjectPosition *target_position,
                            int                        target_speed,
                            int                        maximum_adjustment,
                            int                        distance_limit,
                            int                        target_offset,
                            int                        mode)
{
  int distance;
  int desired_speed;
  int speed_delta;

  distance = 0;
  if(mode == TRACK_OBJECT_MODE_FORWARD)
    {
      distance = (target_position->track_position -
                  current_position->track_position) >>
                 TRACK_OBJECT_DISTANCE_FRACTION_BITS;
    }
  else if(mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
    {
      distance = (target_position->lateral_position -
                  current_position->lateral_position) >>
                 TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
    }
  else if(mode == TRACK_OBJECT_MODE_REVERSE)
    {
      distance = (current_position->track_position -
                  target_position->track_position) >>
                 TRACK_OBJECT_DISTANCE_FRACTION_BITS;
    }
  else if(mode == TRACK_OBJECT_MODE_CROSS_NEGATIVE)
    {
      distance = (target_position->lateral_position -
                  current_position->lateral_position) >>
                 TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
    }

  distance += target_offset;
  if(distance > distance_limit)
    {
      desired_speed = maximum_speed;
    }
  else if(-distance > distance_limit)
    {
      desired_speed = minimum_speed;
    }
  else
    {
      desired_speed = target_speed +
                      scale_speed_distance_towards_zero(distance);
      if(desired_speed > maximum_speed)
        {
          desired_speed = maximum_speed;
        }
      else if(desired_speed < minimum_speed)
        {
          desired_speed = minimum_speed;
        }
    }

  speed_delta = desired_speed - current_speed;
  if(speed_delta > maximum_adjustment)
    {
      return current_speed + maximum_adjustment;
    }

  speed_delta = current_speed - desired_speed;
  if(speed_delta > maximum_adjustment)
    {
      return current_speed - maximum_adjustment;
    }

  return desired_speed;
}

/* Reconstructed from the original function at 0x000196F8. */

#include "track_object_runtime.h"

int
measure_track_object_distance(const TrackObjectPosition *current_position,
                              const TrackObjectPosition *target_position,
                              int                        mode)
{
  int delta;

  if((mode & TRACK_OBJECT_MODE_DIRECTION_BIT) != 0)
    {
      if(mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
        {
          delta = target_position->lateral_position -
                  current_position->lateral_position;
        }
      else
        {
          delta = current_position->lateral_position -
                  target_position->lateral_position;
        }
      return delta >> TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
    }

  if(mode == TRACK_OBJECT_MODE_REVERSE)
    {
      delta = current_position->track_position -
              target_position->track_position;
    }
  else
    {
      delta = target_position->track_position -
              current_position->track_position;
    }
  return delta >> TRACK_OBJECT_DISTANCE_FRACTION_BITS;
}

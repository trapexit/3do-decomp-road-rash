#include "track_object_query.h"
#include "track_traversal_runtime.h"

TrackInteractionObject *
find_track_object_in_direction(TrackInteractionObject *origin_object,
                               unsigned int            collision_mask,
                               int                     relative_mode,
                               int                     maximum_distance)
{
  int direction_or_side;

  if((relative_mode & TRACK_OBJECT_MODE_DIRECTION_BIT) != 0)
    {
      if(relative_mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
        {
          direction_or_side = TRACK_TRAVERSAL_FORWARD;
        }
      else
        {
          direction_or_side = TRACK_TRAVERSAL_REVERSE;
        }
      return find_nearest_lateral_track_object(
        origin_object, collision_mask, direction_or_side);
    }

  if(relative_mode != TRACK_OBJECT_MODE_FORWARD)
    {
      direction_or_side = TRACK_TRAVERSAL_REVERSE;
    }
  else
    {
      direction_or_side = TRACK_TRAVERSAL_FORWARD;
    }
  return find_nearby_track_object_in_direction(
    origin_object, collision_mask, direction_or_side,
    maximum_distance);
}

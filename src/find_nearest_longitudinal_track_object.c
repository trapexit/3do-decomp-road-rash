#include "track_object_query.h"
#include "track_traversal_runtime.h"

int
absolute_int_difference(int first,
                        int second);

TrackInteractionObject *
find_nearest_longitudinal_track_object(TrackInteractionObject *origin,
                                       unsigned int            collision_mask,
                                       int                     maximum_distance)
{
  TrackInteractionObject *forward_candidate;
  TrackInteractionObject *reverse_candidate;
  int forward_distance;
  int reverse_distance;

  forward_candidate = find_nearby_track_object_in_direction(
    origin, collision_mask, TRACK_TRAVERSAL_FORWARD,
    maximum_distance);
  reverse_candidate = find_nearby_track_object_in_direction(
    origin, collision_mask, TRACK_TRAVERSAL_REVERSE,
    maximum_distance);

  if(forward_candidate == 0)
    {
      return reverse_candidate;
    }
  if(reverse_candidate == 0)
    {
      return forward_candidate;
    }

  forward_distance = absolute_int_difference(
    origin->position.track_position,
    forward_candidate->position.track_position);
  reverse_distance = absolute_int_difference(
    origin->position.track_position,
    reverse_candidate->position.track_position);

  if(forward_distance <= reverse_distance)
    {
      return forward_candidate;
    }
  return reverse_candidate;
}

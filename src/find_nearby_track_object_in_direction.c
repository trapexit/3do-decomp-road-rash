#include "track_object_query.h"
#include "track_traversal_runtime.h"

TrackInteractionObject *
find_nearby_track_object_in_direction(TrackInteractionObject *origin_object,
                                      unsigned int            collision_mask,
                                      int                     search_direction,
                                      int                     maximum_distance)
{
  int origin_position;
  unsigned int direction;
  unsigned int origin_topology;
  TrackInteractionObject *candidate;
  RoadSegmentLaneRuntime *origin_lane;
  RoadSegmentLaneRuntime *candidate_lane;
  int distance;

  direction = (unsigned int)search_direction &
              TRACK_OBJECT_DIRECTION_MASK;
  origin_lane = origin_object->track_segment;
  origin_position = origin_object->position.track_position;
  if(origin_lane == 0)
    {
      return 0;
    }

  origin_topology = origin_lane->topology_flags;
  if(maximum_distance < 0)
    {
      maximum_distance = TRACK_OBJECT_UNBOUNDED_SEARCH_DISTANCE;
    }

  if(direction != TRACK_TRAVERSAL_REVERSE)
    {
      candidate = (TrackInteractionObject *)
                  origin_object->list_node.links.next;
      while(candidate->list_node.links.next != 0)
        {
          distance = candidate->position.track_position - origin_position;
          if(distance >= maximum_distance)
            {
              return 0;
            }

          candidate_lane = candidate->track_segment;
          if(candidate_lane != 0 &&
             ((unsigned int)candidate->collision_flags &
              collision_mask) != 0 &&
             (origin_topology & candidate_lane->topology_flags &
              TRACK_OBJECT_TOPOLOGY_MATCH_MASK) != 0)
            {
              return candidate;
            }

          candidate = (TrackInteractionObject *)
                      candidate->list_node.links.next;
        }
    }
  else
    {
      candidate = (TrackInteractionObject *)
                  origin_object->list_node.links.previous;
      while(candidate->list_node.links.previous != 0)
        {
          distance = origin_position - candidate->position.track_position;
          if(distance >= maximum_distance)
            {
              return 0;
            }

          candidate_lane = candidate->track_segment;
          if(candidate_lane != 0 &&
             ((unsigned int)candidate->collision_flags &
              collision_mask) != 0 &&
             (origin_topology & candidate_lane->topology_flags &
              TRACK_OBJECT_TOPOLOGY_MATCH_MASK) != 0)
            {
              return candidate;
            }

          candidate = (TrackInteractionObject *)
                      candidate->list_node.links.previous;
        }
    }

  return 0;
}


TrackInteractionObject *
find_nearest_lateral_track_object(TrackInteractionObject *origin_object,
                                  unsigned int            collision_mask,
                                  int                     lateral_side)
{
  TrackInteractionObject *forward_base;
  TrackInteractionObject *backward_base;
  TrackInteractionObject *forward_candidate;
  TrackInteractionObject *backward_candidate;
  unsigned int desired_side;
  int remaining_span;
  int candidate_side;
  int same_lateral_position;

  desired_side = (unsigned int)lateral_side &
                 TRACK_OBJECT_DIRECTION_MASK;
  forward_base = origin_object;
  backward_base = origin_object;

  do
    {
      remaining_span = TRACK_OBJECT_LATERAL_SEARCH_SPAN -
                       (forward_base->position.track_position -
                        origin_object->position.track_position);
      forward_candidate = find_nearby_track_object_in_direction(
        forward_base, collision_mask, TRACK_TRAVERSAL_FORWARD,
        remaining_span);
      forward_base = forward_candidate;
      if(forward_candidate == 0)
        {
          break;
        }

      candidate_side =
        forward_candidate->position.lateral_position >
        origin_object->position.lateral_position;
      same_lateral_position =
        forward_candidate->position.lateral_position ==
        origin_object->position.lateral_position;
    } while((unsigned int)candidate_side != desired_side ||
            same_lateral_position != 0);

  do
    {
      remaining_span = TRACK_OBJECT_LATERAL_SEARCH_SPAN -
                       (origin_object->position.track_position -
                        backward_base->position.track_position);
      backward_candidate = find_nearby_track_object_in_direction(
        backward_base, collision_mask, TRACK_TRAVERSAL_REVERSE,
        remaining_span);
      backward_base = backward_candidate;
      if(backward_candidate == 0)
        {
          break;
        }

      candidate_side =
        backward_candidate->position.lateral_position >
        origin_object->position.lateral_position;
      same_lateral_position =
        backward_candidate->position.lateral_position ==
        origin_object->position.lateral_position;
    } while((unsigned int)candidate_side != desired_side ||
            same_lateral_position != 0);

  if(forward_candidate == 0)
    {
      return backward_candidate;
    }
  if(backward_candidate == 0)
    {
      return forward_candidate;
    }

  candidate_side =
    forward_candidate->position.lateral_position >
    backward_candidate->position.lateral_position;
  if((unsigned int)candidate_side != desired_side)
    {
      return forward_candidate;
    }
  return backward_candidate;
}

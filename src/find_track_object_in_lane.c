#include "division_helpers.h"
#include "track_object_query.h"

enum TrackObjectLaneQueryConstant
{
  TRACK_OBJECT_LANE_SPACING_FIXED =
    TRACK_OBJECT_LANE_SPACING << TRACK_OBJECT_POSITION_FRACTION_BITS
};

TrackInteractionObject *
find_track_object_in_lane(TrackInteractionObject *object,
                          int                     lane,
                          int                     mode,
                          unsigned int            mask,
                          int                     max_distance)
{
  TrackInteractionObject *candidate;
  RoadSegmentLaneRuntime *track;
  int original_position;
  int lane_position;
  int delta;
  int matches;
  volatile int match_by_position;

  original_position = object->position.track_position;
  lane = (signed char)lane;
  match_by_position = mode & TRACK_OBJECT_MODE_DIRECTION_BIT;

  do
    {
      candidate = find_track_object_in_direction(object, mask, mode, max_distance);
      if(candidate == 0)
        {
          return 0;
        }

      if(match_by_position != 0)
        {
          matches =
            (candidate->position.track_position >>
             TRACK_OBJECT_POSITION_FRACTION_BITS) ==
            (original_position >> TRACK_OBJECT_POSITION_FRACTION_BITS);
        }
      else
        {
          track = candidate->track_segment;
          delta = candidate->position.lateral_position -
                  (track->center_position <<
                   TRACK_OBJECT_POSITION_FRACTION_BITS);
          lane_position = divide_lane_offset_by_spacing(delta);
          if(delta < 0)
            {
              lane_position--;
            }
          matches = lane_position == lane;
        }

      object = candidate;
    } while(!matches);

  return candidate;
}

#include "rw_semantic_data.h"
#include "track_object_query.h"
#include "track_traversal_runtime.h"

enum PacingRiderSearchConstant
{
  PACING_PAIRED_RIDER_COLLISION_MASK = 8
};

RacerEntity *
find_nearby_pacing_rider_in_direction(RacerEntity *rider,
                                      int          direction,
                                      int          maximum_distance)
{
  RacerEntity *search_origin;
  RacerEntity *candidate;
  RacerEntity *challenge;
  RoadSegmentLaneRuntime *lane;
  int accept_off_road_candidate;
  int lateral_position;

  search_origin = rider->paired_state;
  challenge = (RacerEntity *)race_rider_state.challenge_object;
  accept_off_road_candidate =
    challenge != 0 && search_origin == challenge->paired_state;

  for(;;)
    {
      candidate = (RacerEntity *)find_nearby_track_object_in_direction(
        (TrackInteractionObject *)search_origin,
        PACING_PAIRED_RIDER_COLLISION_MASK,
        direction, maximum_distance);
      if(candidate == 0)
        {
          return 0;
        }

      lateral_position =
        candidate->position_x >> TRACK_POSITION_FRACTION_BITS;
      lane = candidate->track_segment;
      if((lane->road_left < lateral_position &&
          lateral_position < lane->road_right) ||
         accept_off_road_candidate)
        {
          return candidate->owner;
        }

      search_origin = candidate;
    }
}

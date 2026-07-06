#include "rw_semantic_data.h"
#include "track_object_query.h"
#include "track_traversal_runtime.h"

enum ChallengeRacerSearchConstant
{
  CHALLENGE_PAIRED_RIDER_COLLISION_MASK = 8,
  CHALLENGE_RACER_MODE_NORMAL = 0,
  CHALLENGE_PLAYER_FINISHED_FLAG = 0x10,
  CHALLENGE_SEARCH_DIRECTION_COUNT = 2
};

RacerEntity *
find_nearby_active_challenge_racer(RacerEntity *player)
{
  RacerEntity *search_origin;
  RacerEntity *candidate;
  RacerEntity *challenge;
  RacerEntity *challenge_racer;
  int direction;
  int search_distance;

  search_origin = player->paired_state;
  direction = TRACK_TRAVERSAL_REVERSE;
  search_distance = TRACK_OBJECT_ANIMATION_FAR_DISTANCE;

  while(direction < CHALLENGE_SEARCH_DIRECTION_COUNT)
    {
      candidate = (RacerEntity *)find_nearby_track_object_in_direction(
        (TrackInteractionObject *)search_origin,
        CHALLENGE_PAIRED_RIDER_COLLISION_MASK,
        direction, search_distance);
      if(candidate != 0)
        {
          challenge =
            (RacerEntity *)race_rider_state.challenge_object;
          if(challenge != 0 &&
             candidate == challenge->paired_state)
            {
              challenge_racer = candidate->owner;
              if(challenge_racer->rider_mode ==
                 CHALLENGE_RACER_MODE_NORMAL)
                {
                  if((player->status_flags &
                      CHALLENGE_PLAYER_FINISHED_FLAG) != 0)
                    {
                      return 0;
                    }
                  return challenge_racer;
                }
            }
          search_origin = candidate;
        }
      else
        {
          direction++;
          search_distance >>= 1;
          search_origin = player->paired_state;
        }
    }

  return 0;
}

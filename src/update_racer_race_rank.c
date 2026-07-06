#include "ai_racer_runtime.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

enum RacerRankUpdateConstant
{
  RACER_RANK_PLAYER_UPDATE_TICKS = 4,
  RACER_RANK_OPPONENT_UPDATE_TICKS = 60,
  RACER_RANK_TOPOLOGY_MASK =
    ROAD_LANE_TOPOLOGY_PRIMARY | ROAD_LANE_TOPOLOGY_SECONDARY
};

int
update_racer_race_rank(AiRacerObject *racer)
{
  AiRacerObject *other;
  RoadSegmentLaneRuntime *other_lane;
  int rank;
  int finish_distance;
  unsigned int topology_flags;
  int rider_index;
  int ranked_rider_count;

  rank = 1;
  topology_flags = racer->track_segment->topology_flags;
  finish_distance = update_racer_finish_distance(racer);

  ranked_rider_count = race_rider_state.rider_count - 1;
  for(rider_index = 0;
      rider_index < ranked_rider_count;
      rider_index++)
    {
      other =
        (AiRacerObject *)race_rider_state.rider_states[rider_index];
      if(other != 0)
        {
          other_lane = other->track_segment;
          if(other_lane != 0 &&
             (other_lane->topology_flags & topology_flags &
              RACER_RANK_TOPOLOGY_MASK) != 0)
            {
              if(other->position.track_position >
                 racer->position.track_position)
                {
                  rank++;
                }
            }
          else if(other->finish_distance < finish_distance)
            {
              rank++;
            }
        }
    }

  racer->race_rank = rank;
  if(racer->owner !=
     (AiRacerObject *)race_rider_state.player_object)
    {
      racer->next_rank_update_tick =
        racer->next_update_tick + RACER_RANK_OPPONENT_UPDATE_TICKS;
    }
  else
    {
      racer->next_rank_update_tick =
        racer->next_update_tick + RACER_RANK_PLAYER_UPDATE_TICKS;
    }
  return rank;
}

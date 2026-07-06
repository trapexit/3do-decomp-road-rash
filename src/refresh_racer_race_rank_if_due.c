#include "ai_racer_runtime.h"

int
refresh_racer_race_rank_if_due(AiRacerObject *racer)
{
  if(racer->next_update_tick < racer->next_rank_update_tick)
    {
      return racer->race_rank;
    }
  return update_racer_race_rank(racer);
}

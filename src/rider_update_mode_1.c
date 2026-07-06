#include "ai_collision_avoidance_runtime.h"

int
rider_update_mode_1(int rider_address)
{
  if(select_ai_collision_avoidance_target(
       (AiRacerObject *)rider_address) == 0)
    {
      select_ai_curvature_lane_target((AiRacerObject *)rider_address);
    }
  return apply_ai_rider_navigation_control((RacerEntity *)rider_address);
}

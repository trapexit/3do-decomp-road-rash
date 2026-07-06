#include "racer_runtime.h"

int
apply_bike_health_damage(RacerEntity *rider,
                         int          damage)
{
  rider->current_bike_health -= damage;
  if(rider->current_bike_health <= 0)
    {
      rider->current_bike_health = 0;
    }

  return rider->current_bike_health;
}

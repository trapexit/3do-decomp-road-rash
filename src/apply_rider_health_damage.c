#include "racer_runtime.h"

enum
{
  RECOVERY_HEALTH_DAMAGE_SHIFT = 2,
  CURRENT_HEALTH_DAMAGE_SHIFT = 6,
  INTEGER_SIGN_SHIFT = 31,
  HALF_HEALTH_SHIFT = 1
};

int
apply_rider_health_damage(RacerEntity *rider,
                          int          damage)
{
  RiderRuntimeSlot *runtime;
  int maximum_health;
  int minimum_recovery_health;

  runtime = rider->paired_state->runtime;

  runtime->recovery_health_ceiling -=
    damage << RECOVERY_HEALTH_DAMAGE_SHIFT;

  maximum_health = runtime->maximum_health;
  minimum_recovery_health =
    (maximum_health +
     (int)((unsigned int)maximum_health >> INTEGER_SIGN_SHIFT)) >>
    HALF_HEALTH_SHIFT;
  if(minimum_recovery_health > runtime->recovery_health_ceiling)
    {
      runtime->recovery_health_ceiling = minimum_recovery_health;
    }

  runtime->current_health -= damage << CURRENT_HEALTH_DAMAGE_SHIFT;
  if(runtime->current_health <= 0)
    {
      runtime->current_health = 0;
    }

  return runtime->current_health;
}

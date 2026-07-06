#include "racer_runtime.h"
#include "rider_physics_configuration.h"

enum RiderTransmissionGearConstant
{
  RIDER_TRANSMISSION_FIRST_GEAR = 0,
  RIDER_TRANSMISSION_LAST_GEAR =
    RIDER_GEAR_RUNTIME_PROFILE_COUNT - 1
};

void
update_rider_transmission_gear(RiderTransmissionRuntime *transmission)
{
  RacerEntity *rider;
  RiderGearProfile *profile;
  int gear;
  int forward_speed;

  rider = (RacerEntity *)transmission;
  gear = transmission->current_gear;
  profile = &transmission->profiles[gear];
  forward_speed = rider->velocity.forward;

  if(profile->upshift_speed < forward_speed)
    {
      gear++;
      if(gear > RIDER_TRANSMISSION_LAST_GEAR)
        {
          gear = RIDER_TRANSMISSION_LAST_GEAR;
        }
      transmission->current_gear = gear;
      return;
    }

  if(profile->downshift_speed <= forward_speed)
    {
      return;
    }

  gear--;
  if(gear < RIDER_TRANSMISSION_FIRST_GEAR)
    {
      gear = RIDER_TRANSMISSION_FIRST_GEAR;
    }
  transmission->current_gear = gear;
}

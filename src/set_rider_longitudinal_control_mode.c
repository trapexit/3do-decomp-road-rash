#include "racer_runtime.h"
#include "rider_physics_configuration.h"

enum RiderLongitudinalControlMode
{
  RIDER_LONGITUDINAL_CONTROL_ACCELERATE = 1,
  RIDER_LONGITUDINAL_CONTROL_BRAKE = 2
};

int
set_rider_longitudinal_control_mode(RacerEntity *rider,
                                    int          mode)
{
  RiderPhysicsConfiguration *configuration;

  configuration = (RiderPhysicsConfiguration *)rider;
  if(mode == RIDER_LONGITUDINAL_CONTROL_ACCELERATE)
    {
      return approach_rider_acceleration_target(
        rider, configuration->maximum_forward_control);
    }
  if(mode == RIDER_LONGITUDINAL_CONTROL_BRAKE)
    {
      return approach_rider_braking_target(
        rider, configuration->maximum_reverse_control);
    }
  if(configuration->longitudinal_control < 0)
    {
      return approach_rider_braking_target(rider, 0);
    }
  return approach_rider_acceleration_target(rider, 0);
}

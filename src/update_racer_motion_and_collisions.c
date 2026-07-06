#include "racer_runtime.h"
#include "rider_physics_configuration.h"

void
update_rider_transmission_gear(RiderTransmissionRuntime *transmission);
void
integrate_racer_motion_step(RacerEntity *racer);
void
resolve_racer_road_boundary_collision(RacerEntity *racer);
void
update_racer_vertical_surface_motion(char *racer);

void
update_racer_motion_and_collisions(RacerEntity *racer)
{
  int surface_zone;

  surface_zone = update_rider_traction_and_heading(racer);
  update_racer_forward_velocity(racer, surface_zone);
  update_rider_transmission_gear((RiderTransmissionRuntime *)racer);
  integrate_racer_motion_step(racer);
  resolve_racer_road_boundary_collision(racer);
  update_racer_vertical_surface_motion((char *)racer);
}

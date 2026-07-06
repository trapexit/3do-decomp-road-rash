#include "racer_runtime.h"

enum PassiveRacerMotionConstant
{
  PASSIVE_RACER_SURFACE_UPDATE_DISABLED_CLASS = 0x10
};

void
integrate_track_object_motion(RacerEntity *object);
void
resolve_racer_road_boundary_collision(RacerEntity *racer);
void
update_racer_vertical_surface_motion(char *racer);

void
update_passive_racer_motion_and_collisions(RacerEntity *racer)
{
  int surface_zone = 0;

  if(racer->surface_contact_scale_8_8 == 0)
    {
      racer->drive_acceleration = 0;
      racer->steering_force = 0;
      racer->slip_amount = 0;
    }

  if(racer->surface_contact_scale_8_8 != 0)
    {
      surface_zone = classify_rider_surface_zone((char *)racer->track_segment,
                                                 racer->position_x);
    }

  update_racer_forward_velocity(racer, surface_zone);
  integrate_track_object_motion(racer);
  resolve_racer_road_boundary_collision(racer);

  if(racer->collision_class !=
     PASSIVE_RACER_SURFACE_UPDATE_DISABLED_CLASS)
    {
      update_racer_vertical_surface_motion((char *)racer);
    }
}

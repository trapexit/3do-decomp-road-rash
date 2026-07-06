#include "rw_semantic_data.h"
#include "track_object_runtime.h"

enum TrackObjectRuntimeInitializationConstant
{
  TRACK_OBJECT_POOL_CAPACITY = 16,
  TRACK_OBJECT_BASE_SCALE = -25,
  TRACK_OBJECT_DEFAULT_SCALE = 6,
  TRACK_OBJECT_SPAWN_PERIOD_WORKSPACE = 4,
  TRACK_OBJECT_FORWARD_SPAWN_TIMER_WORKSPACE = 5,
  TRACK_OBJECT_REVERSE_SPAWN_TIMER_WORKSPACE = 6
};

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);
void
load_car_animation_resources(void);
void
configure_track_simulation_scale(int scale);

void
initialize_track_object_runtime(void)
{
  if(runtime_feature_flags.track_objects_enabled.value == 0)
    {
      track_simulation_globals.active_object = 0;
      return;
    }

  track_simulation_globals.active_object =
    create_intrusive_object_pool(
      sizeof(TrackObject), TRACK_OBJECT_POOL_CAPACITY, 0);
  if(track_simulation_globals.active_object == 0)
    {
      return;
    }
  load_car_animation_resources();
  configure_track_simulation_scale(TRACK_OBJECT_BASE_SCALE);
  configure_track_simulation_scale(TRACK_OBJECT_DEFAULT_SCALE);
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_SPAWN_TIMER_WORKSPACE] = 0;
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_REVERSE_SPAWN_TIMER_WORKSPACE] =
    -track_simulation_globals.active_workspace[
      TRACK_OBJECT_SPAWN_PERIOD_WORKSPACE];
}

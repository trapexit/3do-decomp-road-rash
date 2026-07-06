#include "intrusive_list.h"
#include "rw_semantic_data.h"

void
unload_car_animation_resources(void);

void
destroy_track_object_runtime(void)
{
  IntrusiveListStorage *object_pool;

  object_pool = (IntrusiveListStorage *)
                track_simulation_globals.active_object;
  if(object_pool != 0)
    {
      destroy_intrusive_object_pool(object_pool);
      unload_car_animation_resources();
    }

  track_simulation_globals.active_object = 0;
}

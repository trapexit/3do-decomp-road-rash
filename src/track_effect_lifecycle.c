#include "intrusive_list.h"
#include "track_effect_runtime.h"

void
configure_track_simulation_scale(int scale);

int
dispose_track_effect_pools(void)
{
  void *object_pool;
  void *state_pool;

  object_pool = track_effect_pools.child_pool;
  state_pool = track_effect_pools.parent_pool;
  track_effect_pools.child_pool = 0;
  track_effect_pools.parent_pool = 0;

  if(object_pool != 0)
    {
      destroy_intrusive_object_pool((IntrusiveListStorage *)object_pool);
    }
  if(state_pool != 0)
    {
      destroy_intrusive_object_pool((IntrusiveListStorage *)state_pool);
    }
  return 0;
}


void
apply_track_effect_spawn_record(TrackEffectSpawnRecord *record,
                                RoadSegmentLaneRuntime *origin_segment)
{
  spawn_track_effect_parent(record, origin_segment);
  configure_track_simulation_scale(record->simulation_scale);
}

#include "intrusive_list.h"
#include "rw_semantic_data.h"
#include "track_effect_runtime.h"

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node);

int
initialize_track_effect_pools(void)
{
  void *child_pool;
  void *parent_pool;

  child_pool = track_effect_pools.child_pool;
  parent_pool = track_effect_pools.parent_pool;
  if(child_pool != 0 && parent_pool != 0)
    {
      return (int)child_pool;
    }

  if(child_pool != 0)
    {
      destroy_intrusive_object_pool((IntrusiveListStorage *)child_pool);
    }
  if(parent_pool != 0)
    {
      destroy_intrusive_object_pool((IntrusiveListStorage *)parent_pool);
    }
  track_effect_pools.child_pool = 0;
  track_effect_pools.parent_pool = 0;

  child_pool = create_intrusive_object_pool(
    TRACK_EFFECT_CHILD_OBJECT_SIZE,
    TRACK_EFFECT_CHILD_POOL_CAPACITY, 0);
  if(child_pool == 0)
    {
      return 0;
    }

  parent_pool = create_intrusive_object_pool(
    sizeof(TrackEffectObject),
    TRACK_EFFECT_PARENT_POOL_CAPACITY, 0);
  if(parent_pool == 0)
    {
      destroy_intrusive_object_pool((IntrusiveListStorage *)child_pool);
      return 0;
    }

  track_effect_pools.child_pool = child_pool;
  track_effect_pools.parent_pool = parent_pool;
  return (int)child_pool;
}

#include "rw_semantic_data.h"

int
deactivate_road_particle_slot(RoadParticleSlot *slot)
{
  int count;

  count = particle_resource_globals.active_count;
  if(count <= 0)
    {
      return (int)slot;
    }

  slot->active_flags &= ~1;
  count--;
  particle_resource_globals.active_count = count;
  return count;
}

#include "bss_late_data.h"
#include "rw_semantic_data.h"

int
deactivate_road_particle_slot(RoadParticleSlot *slot);

void
clear_road_particle_resources(void)
{
  int count;
  int i;
  RoadParticleSlot *entry;

  particle_resource_globals.node_slots[0] = 0;
  road_surface_render_globals.primary_ccb = 0;
  particle_resource_globals.node_slots[1] = 0;
  road_surface_render_globals.fallback_ccb = 0;
  particle_resource_globals.palette_node = 0;

  for(i = 0; i < PARTICLE_RESOURCE_COUNT; i++)
    {
      particle_resource_globals.resource_nodes[i] = 0;
      particle_resource_globals.cel_pointers[i] = 0;
    }

  count = particle_resource_globals.active_count;
  entry = gRoadSegmentEffectWorkspace.particle_effects.particle_slots;
  while(count > 0)
    {
      deactivate_road_particle_slot(entry);
      entry++;
      count -= 1;
    }
}

#include "application_runtime.h"
#include "front_end_cel_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

RoadParticleSlot *
allocate_road_particle_slot(void);
MemoryHandle *
RSRC_LoadResource(uint32 resource_type,
                  int32  resource_id,
                  uint32 options);

enum RoadParticleResourceConstant
{
  ROAD_SURFACE_PRIMARY_RESOURCE_ID = 0x49,
  ROAD_SURFACE_FALLBACK_RESOURCE_ID = 0x4A,
  ROAD_PARTICLE_FIRST_RESOURCE_ID = 0x4B,
  ROAD_PARTICLE_PALETTE_RESOURCE_ID = 1,
  ROAD_PARTICLE_PALETTE_DATA_OFFSET = 0x0C,
  ROAD_PARTICLE_INITIAL_SLOT_COUNT = 50
};

void
load_road_particle_resources(void)
{
  MemoryHandle **node_slot;
  CCB **ccb_slot;
  MemoryHandle *node;
  CCB *ccb;
  void *palette;
  int index;

  node = RSRC_LoadResource(
    CANS_RESOURCE_TYPE_CEL, ROAD_SURFACE_PRIMARY_RESOURCE_ID,
    RESOURCE_LOAD_OPTION_KEEP_BUSY);
  particle_resource_globals.node_slots[0] = node;
  if(node != 0)
    {
      road_surface_render_globals.primary_ccb =
        parse_cans_cel_chunk_stream(
          (CansChunkHeader *)node->data, 0);
    }
  else
    {
      road_surface_render_globals.primary_ccb = 0;
    }

  node = RSRC_LoadResource(
    CANS_RESOURCE_TYPE_CEL, ROAD_SURFACE_FALLBACK_RESOURCE_ID,
    RESOURCE_LOAD_OPTION_KEEP_BUSY);
  particle_resource_globals.node_slots[1] = node;
  if(node != 0)
    {
      road_surface_render_globals.fallback_ccb =
        parse_cans_cel_chunk_stream(
          (CansChunkHeader *)node->data, 0);
    }
  else
    {
      road_surface_render_globals.fallback_ccb = 0;
    }

  node = RSRC_LoadResource(
    CANS_RESOURCE_TYPE_PALETTE,
    ROAD_PARTICLE_PALETTE_RESOURCE_ID,
    RESOURCE_LOAD_OPTION_KEEP_BUSY);
  particle_resource_globals.palette_node = node;
  if(node == 0)
    {
      return;
    }

  palette = (char *)node->data + ROAD_PARTICLE_PALETTE_DATA_OFFSET;
  ccb_slot = (CCB **)particle_resource_globals.cel_pointers;
  node_slot = (MemoryHandle **)particle_resource_globals.resource_nodes;

  for(index = 0; index < PARTICLE_RESOURCE_COUNT; index++)
    {
      *ccb_slot = 0;
      node = RSRC_LoadResource(
        CANS_RESOURCE_TYPE_CEL,
        index + ROAD_PARTICLE_FIRST_RESOURCE_ID,
        RESOURCE_LOAD_OPTION_KEEP_BUSY);
      *node_slot = node;
      if(node == 0)
        {
          return;
        }

      ccb = parse_cans_cel_chunk_stream(
        (CansChunkHeader *)node->data, 0);
      *ccb_slot = ccb;
      ccb->ccb_PLUTPtr = palette;

      ccb_slot++;
      node_slot++;
    }

  particle_resource_globals.active_count = 0;
  for(index = 0; index < ROAD_PARTICLE_INITIAL_SLOT_COUNT; index++)
    {
      allocate_road_particle_slot();
    }

  particle_resource_globals.render_state[1] = 0;
}

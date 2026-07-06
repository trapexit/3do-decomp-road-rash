#include "family_resource_runtime.h"
#include "road_geometry.h"
#include "road_texture_cache_residency.h"
#include "rw_semantic_data.h"

/* Family groups are relocated into reusable buffers.  The oracle normally
   completes its scheduled load before a projected depth cache is reused, but
   source-build timing can expose the same cache while that buffer changes
   family.  Track semantic ownership beside the raw CLGP pointers so a stable
   render-node depth cannot preserve a pointer into replacement contents. */
int
road_texture_cache_pool_index(const RoadTextureCache *cache)
{
  unsigned long cache_address;
  unsigned long pool_address;
  unsigned long pool_end;
  unsigned long offset;
  int cache_index;

  if(cache == 0)
    {
      return -1;
    }

  cache_address = (unsigned long)cache;
  pool_address = (unsigned long)&gRoadTextureCachePool[0];
  pool_end = (unsigned long)
             &gRoadTextureCachePool[ROAD_TEXTURE_CACHE_COUNT];
  if(cache_address < pool_address || cache_address >= pool_end)
    {
      return -1;
    }

  offset = cache_address - pool_address;
  cache_index = (int)(offset / sizeof(gRoadTextureCachePool[0]));
  if(cache != &gRoadTextureCachePool[cache_index])
    {
      return -1;
    }
  return cache_index;
}


void
clear_road_texture_resource_binding(RoadTextureBindingState *state,
                                    int                      slot)
{
  /* Internal callers obtain state from a validated cache-pool index and
     derive slot from bounded placement/side enums before reaching here. */
  state->resource_selectors[slot] = 0;
  state->resource_generations[slot] = 0;
}


int
road_texture_resource_binding_matches(
  const RoadTextureBindingState       *state,
  int                                  slot,
  const FamilyResourceBindingIdentity *identity)
{
  return state->resource_selectors[slot] ==
           (unsigned char)identity->selector &&
         state->resource_generations[slot] == identity->generation;
}


void
record_road_texture_resource_binding(
  RoadTextureBindingState             *state,
  int                                  slot,
  const FamilyResourceBindingIdentity *identity)
{
  state->resource_selectors[slot] = (unsigned char)identity->selector;
  state->resource_generations[slot] = identity->generation;
}


static
int
road_surface_resource_binding_identity(unsigned int                   selector,
                                       FamilyResourceBindingIdentity *identity)
{
  unsigned int generation;
  unsigned int group_index;
  int family_id;

  identity->selector = 0;
  identity->generation = 0;

  if(selector == ROAD_SURFACE_RESOURCE_MISSING)
    {
      identity->selector = selector;
      return 1;
    }

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  family_id =
    family_resource_globals.resident_family_ids[group_index];
  generation = family_resource_generations[group_index];
  if(family_id <= 0 || generation == 0)
    {
      return 0;
    }

  identity->selector = selector;
  identity->generation = generation;
  return 1;
}


void
record_road_surface_cache_residency(RoadTextureBindingState      *state_,
                                  const RoadSegmentLaneRuntime *lane_)
{
  FamilyResourceBindingIdentity identity;
  int side;

  if((state_ == NULL) || (lane_ == NULL))
    return;

  for(side = ROAD_SIDE_LEFT; side < ROAD_SIDE_COUNT; side++)
    {
      road_surface_resource_binding_identity(lane_->surface_resource_ids[side], &identity);
      record_road_texture_resource_binding(state_,
                                          (ROAD_TEXTURE_BINDING_SURFACE_LEFT + side),
                                          &identity);
      road_surface_resource_binding_identity(lane_->edge_resource_ids[side], &identity);
      record_road_texture_resource_binding(state_,
                                          (ROAD_TEXTURE_BINDING_EDGE_LEFT + side),
                                          &identity);
    }

  if((lane_->geometry_mode == ROAD_GEOMETRY_MODE_EDGE) ||
     (lane_->geometry_mode == ROAD_GEOMETRY_MODE_CURVED))
    {
      road_surface_resource_binding_identity(lane_->surface_selector, &identity);
      record_road_texture_resource_binding(state_, ROAD_TEXTURE_BINDING_OBJECT_CEL, &identity);
    }

  state_->surface_resources_bound = 1;
  state_->surface_geometry_mode = (unsigned int)lane_->geometry_mode;
}

#ifndef ROADRASH_ROAD_TEXTURE_CACHE_RESIDENCY_H
#define ROADRASH_ROAD_TEXTURE_CACHE_RESIDENCY_H

#include "bss_early_data.h"
#include "bss_late_data.h"

int
road_texture_cache_pool_index(const RoadTextureCache *cache);
void
clear_road_texture_resource_binding(RoadTextureBindingState *state,
                                    int                      slot);
int
road_texture_resource_binding_matches(
  const RoadTextureBindingState       *state,
  int                                  slot,
  const FamilyResourceBindingIdentity *identity);
void
record_road_texture_resource_binding(
  RoadTextureBindingState             *state,
  int                                  slot,
  const FamilyResourceBindingIdentity *identity);
// Use the binding state paired with the projected side's cache. NULL inputs are no-ops.
void
record_road_surface_cache_residency(RoadTextureBindingState      *state_,
                                  const RoadSegmentLaneRuntime *lane_);

#endif

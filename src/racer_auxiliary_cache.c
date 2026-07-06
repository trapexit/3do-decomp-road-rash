#include "family_resource_runtime.h"
#include "racer_auxiliary_cache.h"
#include "rw_semantic_data.h"

enum RacerAuxiliaryCacheConstant
{
  RACER_AUXILIARY_RESOURCE_TYPE = 4,
  RACER_AUXILIARY_RESOURCE_SLOT = 1
};

/*
 * Keep the 32-bit generation counters naturally aligned, but store the
 * eight-bit selectors separately.  FamilyResourceBindingIdentity is padded
 * to eight bytes on ARM; the split representation retains every bit while
 * avoiding 408 bytes of repeated alignment in the 68-by-2 cache table.
 */
static unsigned int
  racer_auxiliary_binding_generations[ROAD_TEXTURE_CACHE_COUNT]
  [ROAD_STATIC_OBJECT_CACHE_COUNT];
static unsigned char
  racer_auxiliary_binding_selectors[ROAD_TEXTURE_CACHE_COUNT]
  [ROAD_STATIC_OBJECT_CACHE_COUNT];

void
reset_all_racer_auxiliary_cache_bindings(void)
{
  int cache_index;
  int entry_index;

  for(cache_index = 0;
      cache_index < ROAD_TEXTURE_CACHE_COUNT;
      cache_index++)
    {
      for(entry_index = 0;
          entry_index < ROAD_STATIC_OBJECT_CACHE_COUNT;
          entry_index++)
        {
          racer_auxiliary_binding_selectors
          [cache_index][entry_index] = 0;
          racer_auxiliary_binding_generations
          [cache_index][entry_index] = 0;
          gRoadTextureCachePool[cache_index]
          .racer_auxiliary_animations[entry_index] = 0;
        }
    }
}


static
int
racer_auxiliary_binding_cache_index(RoadTextureCache *cache,
                                    int               entry_index)
{
  int cache_index;

  if(entry_index < 0 ||
     entry_index >= ROAD_STATIC_OBJECT_CACHE_COUNT)
    {
      return -1;
    }

  for(cache_index = 0;
      cache_index < ROAD_TEXTURE_CACHE_COUNT;
      cache_index++)
    {
      if(cache == &gRoadTextureCachePool[cache_index])
        {
          return cache_index;
        }
    }

  return -1;
}


static
int
current_racer_auxiliary_binding_identity(unsigned int                   selector,
                                         FamilyResourceBindingIdentity *identity)
{
  unsigned int generation;
  unsigned int group_index;
  int family_id;

  identity->selector = 0;
  identity->generation = 0;

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  if(group_index >=
     (unsigned int)family_resource_globals.group_count)
    {
      return 0;
    }

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
invalidate_racer_auxiliary_animation(RoadTextureCache *cache,
                                     int               entry_index)
{
  int cache_index;

  if(cache == 0 || entry_index < 0 ||
     entry_index >= ROAD_STATIC_OBJECT_CACHE_COUNT)
    {
      return;
    }

  cache->racer_auxiliary_animations[entry_index] = 0;
  cache_index = racer_auxiliary_binding_cache_index(cache, entry_index);
  if(cache_index >= 0)
    {
      racer_auxiliary_binding_selectors[cache_index][entry_index] = 0;
      racer_auxiliary_binding_generations[cache_index][entry_index] = 0;
    }
}


CansAnimationData *
resolve_racer_auxiliary_animation(RoadTextureCache *cache,
                                  int               entry_index,
                                  unsigned int      selector)
{
  FamilyResourceBindingIdentity current;
  CansAnimationData *animation;
  int cache_index;

  if(cache == 0 || entry_index < 0 ||
     entry_index >= ROAD_STATIC_OBJECT_CACHE_COUNT)
    {
      return 0;
    }

  cache_index = racer_auxiliary_binding_cache_index(cache, entry_index);
  if(current_racer_auxiliary_binding_identity(
       selector, &current) == 0)
    {
      invalidate_racer_auxiliary_animation(cache, entry_index);
      return 0;
    }

  if(cache_index >= 0 &&
     racer_auxiliary_binding_selectors[cache_index][entry_index] ==
       current.selector &&
     racer_auxiliary_binding_generations[cache_index][entry_index] ==
       current.generation &&
    cache->racer_auxiliary_animations[entry_index] != 0)
    {
      return cache->racer_auxiliary_animations[entry_index];
    }

  /* A cache outside the renderer pool has no durable identity sidecar. */
  cache->racer_auxiliary_animations[entry_index] = 0;
  animation = (CansAnimationData *)lookup_family_resource_child_for_render(
    (FamilyResourceSelector)selector,
    RACER_AUXILIARY_RESOURCE_TYPE,
    RACER_AUXILIARY_RESOURCE_SLOT);
  if(cache_index < 0)
    {
      return animation;
    }
  if(animation == 0)
    {
      racer_auxiliary_binding_selectors[cache_index][entry_index] = 0;
      racer_auxiliary_binding_generations[cache_index][entry_index] = 0;
      return 0;
    }
  cache->racer_auxiliary_animations[entry_index] = animation;
  racer_auxiliary_binding_selectors[cache_index][entry_index] =
    (unsigned char)current.selector;
  racer_auxiliary_binding_generations[cache_index][entry_index] =
    current.generation;
  return animation;
}

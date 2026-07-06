#ifndef ROADRASH_RACER_AUXILIARY_CACHE_H
#define ROADRASH_RACER_AUXILIARY_CACHE_H

#include "bss_early_data.h"

void
reset_all_racer_auxiliary_cache_bindings(void);
CansAnimationData *
resolve_racer_auxiliary_animation(RoadTextureCache *cache,
                                  int               entry_index,
                                  unsigned int      selector);
void
invalidate_racer_auxiliary_animation(RoadTextureCache *cache,
                                     int               entry_index);

#endif

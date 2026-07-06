#ifndef ROADRASH_RIDER_VISUAL_RESOURCE_BINDING_H
#define ROADRASH_RIDER_VISUAL_RESOURCE_BINDING_H

#include "family_resource_binding.h"
#include "rider_visual_runtime.h"

void
reset_all_rider_visual_resource_bindings(void);
void
reset_rider_visual_resource_binding(RiderVisualObject *rider);
void
record_rider_visual_resource_binding(RiderVisualObject                   *rider,
                                     const FamilyResourceBindingIdentity *identity);
int
refresh_rider_visual_resource_binding(RiderVisualObject *rider);

#endif

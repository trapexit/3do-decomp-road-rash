#ifndef ROADRASH_FAMILY_RESOURCE_BINDING_H
#define ROADRASH_FAMILY_RESOURCE_BINDING_H

#include "bss_early_data.h"
#include "family_resource_runtime.h"

FamilyResourceValue
lookup_family_resource_child_with_identity(FamilyResourceSelector         selector,
                                           int                            family_index,
                                           int                            child_index,
                                           FamilyResourceBindingIdentity *identity);
FamilyResourceValue
lookup_family_resource_child_for_render_with_identity(FamilyResourceSelector         selector,
                                                      int                            family_index,
                                                      int                            child_index,
                                                      FamilyResourceBindingIdentity *identity);
/* The caller must hold the family-resource render barrier while validating
   and for the complete lifetime of any borrowed pointer it subsequently
   dereferences.  Validation deliberately does not acquire the group lock:
   loaders reserve that lock before waiting for the render barrier. */
int
family_resource_binding_identity_is_current(const FamilyResourceBindingIdentity *identity);
int
family_resource_binding_values_are_current(FamilyResourceSelector selector,
                                           unsigned int generation);
int
try_acquire_current_family_resource_binding(const FamilyResourceBindingIdentity *identity);
void
release_current_family_resource_binding(const FamilyResourceBindingIdentity *identity);

#endif

#include "family_resource_runtime.h"

int
release_family_resource_lock(FamilyResourceLock *lock)
{
  *lock = FAMILY_RESOURCE_LOCK_FREE;
  return FAMILY_RESOURCE_LOCK_FREE;
}

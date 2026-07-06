#include "family_resource_runtime.h"

int
try_acquire_family_resource_lock(FamilyResourceLock *lock,
                                 int                 requested_state)
{
  if(*lock == requested_state)
    {
      return requested_state == FAMILY_RESOURCE_LOCK_FREE;
    }

  *lock = requested_state;
  return requested_state;
}

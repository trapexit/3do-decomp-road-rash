#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

int
load_family_resource_synchronously(int family_id,
                                   int group_index)
{
  FamilyResourceWorkerRequest request;
  int result;

  if(family_resource_globals.resident_family_ids[group_index] ==
     family_id)
    {
      return 0;
    }

  if(try_acquire_family_resource_lock(
       &family_resource_globals.group_locks[group_index],
       FAMILY_RESOURCE_LOCK_ACQUIRED) !=
     FAMILY_RESOURCE_LOCK_ACQUIRED)
    {
      return 1;
    }

  request.mode = FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE;
  request.group_index = group_index;
  request.family_id = family_id;

  if(acquire_family_resource_render_barrier() == 0)
    {
      release_family_resource_lock(
        &family_resource_globals.group_locks[group_index]);
      return 1;
    }
  seek_family_resource(&request);

  if(family_resource_globals.last_seek_result != 0)
    {
      mark_family_resource_group_unavailable(group_index, family_id);
      release_family_resource_lock(
        &family_resource_globals.group_locks[group_index]);
      release_family_resource_render_barrier();
      return FAMILY_RESOURCE_SYNC_SEEK_FAILED;
    }

  result = (int)load_and_fixup_family_resource_group(&request);
  release_family_resource_lock(
    &family_resource_globals.group_locks[group_index]);
  release_family_resource_render_barrier();
  return result;
}

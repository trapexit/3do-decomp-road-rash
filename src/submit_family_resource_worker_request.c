#ifndef ROADRASH_FAMILY_RESOURCE_HOST_TEST
  #include "task.h"
#else
int
SendSignal(int task_item,
           int signal_mask);
#endif

#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

int
submit_family_resource_worker_request(const struct FamilyResourceRequest *request)
{
  FamilyResourceWorkerRequest *mailbox;
  FamilyResourceLock *lock;
  int current_id;
  int family_id;
  int group_index;
  int signal_result;

  if(family_resource_worker_is_ready() == 0)
    {
      return 1;
    }
  if(family_resource_worker_request_pending != 0)
    {
      return 1;
    }

  family_id = request->family_id;
  group_index = request->group_index;
  current_id =
    family_resource_globals.resident_family_ids[group_index];

  if(current_id == family_id || current_id == -family_id)
    {
      return 0;
    }

  lock = &family_resource_globals.group_locks[group_index];
  if(request->load_mode == FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
    {
      if(try_acquire_family_resource_lock(
           lock, FAMILY_RESOURCE_LOCK_ACQUIRED) !=
         FAMILY_RESOURCE_LOCK_ACQUIRED)
        {
          return 1;
        }
    }
  else
    {
      if(try_acquire_family_resource_lock(
           lock, FAMILY_RESOURCE_LOCK_ACQUIRED) ==
         FAMILY_RESOURCE_LOCK_FREE)
        {
          return 1;
        }
      release_family_resource_lock(lock);
    }

  mailbox = (FamilyResourceWorkerRequest *)
            family_resource_globals.worker_request;
  mailbox->reserved_00 = 0;
  mailbox->family_id = family_id;
  mailbox->group_index = group_index;
  mailbox->mode = request->load_mode;

  family_resource_worker_request_pending = 1;
  signal_result = SendSignal(
    family_resource_globals.worker_task_item,
    family_resource_globals.wake_signal);
  if(signal_result < 0)
    {
      family_resource_worker_request_pending = 0;
      if(request->load_mode ==
         FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
        {
          release_family_resource_lock(lock);
        }
      return 1;
    }
  return 0;
}

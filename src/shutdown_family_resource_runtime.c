#ifndef ROADRASH_FAMILY_RESOURCE_WORKER_HOST_TEST
  #include "platform.h"
  #include "task.h"
#else
#ifndef KERNELNODE
  #define KERNELNODE 1
#endif
#ifndef TASKNODE
  #define TASKNODE 2
#endif
int
DeleteThread(int thread_item);
int
SendSignal(int task_item,
           int signal_mask);
void *
CheckItem(int item,
          int folio,
          int type);
void
Yield(void);
#endif

#include "family_resource_runtime.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
RSRC_DisposeFileResources(int resource_file_item);

int
shutdown_family_resource_runtime(void)
{
  FamilyResourceWorkerRequest request;
  long delete_result;
  int group_index;
  int worker_task_item;
  int result;

  family_resource_worker_startup_status =
    FAMILY_RESOURCE_WORKER_NOT_READY;
  family_resource_worker_shutdown_requested = 1;
  family_resource_globals.worker_running = 0;
  worker_task_item = family_resource_globals.worker_task_item;
  if(worker_task_item > 0 &&
     family_resource_globals.wake_signal != 0)
    {
      SendSignal(worker_task_item,
                 family_resource_globals.wake_signal);
    }

  /*
   * DeleteThread is authoritative when it succeeds.  If it races with the
   * worker's own exit and reports an error while the task is still live,
   * wait for that already-requested exit before releasing worker-owned
   * tables.  Every caller stops the renderer before family teardown, so the
   * worker cannot remain blocked behind an active renderer at this point.
   * Returning with a live worker would leak the tables and semaphore when
   * the next race overwrites the runtime state.
   */
  if(worker_task_item > 0)
    {
      delete_result = DeleteThread(worker_task_item);
      if(delete_result < 0)
        {
          while(CheckItem(worker_task_item, KERNELNODE, TASKNODE) != 0)
            {
              Yield();
            }
        }
    }
  family_resource_globals.worker_task_item = 0;
  family_resource_globals.wake_signal = 0;
  family_resource_worker_startup_signal = 0;
  family_resource_worker_startup_task_item = 0;

  /* A mode-3 request reserves its group before the worker runs.  Release
     that reservation before taking the global barrier so a renderer that
     already owns the barrier cannot remain blocked behind a deleted
     worker. */
  if(family_resource_worker_request_pending != 0)
    {
      request = *(FamilyResourceWorkerRequest *)
                family_resource_globals.worker_request;
      if(request.mode == FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
        {
          release_family_resource_lock(
            &family_resource_globals.group_locks[
              request.group_index]);
        }
      family_resource_worker_request_pending = 0;
    }
  if(family_resource_globals.group_count == 0)
    {
      goto dispose_barrier;
    }
  if(acquire_family_resource_render_barrier() == 0)
    {
      return -1;
    }
  family_resource_globals.request_write_index = 0;
  family_resource_globals.request_read_index = 0;
  family_resource_request_count = 0;
  group_index = 0;
  while(group_index < family_resource_globals.group_count)
    {
      if(family_resource_globals.group_tables[group_index] != 0)
        {
          MEM_DisposePointer(
            family_resource_globals.group_tables[group_index]);
          family_resource_globals.group_tables[group_index] = 0;
        }
      family_resource_globals.resident_family_ids[group_index] = -1;
      family_resource_globals.group_locks[group_index] = 0;
      family_resource_generations[group_index] = 0;
      family_resource_loaded_bytes[group_index] = 0U;
      group_index++;
    }
  while(group_index < FAMILY_GROUP_COUNT)
    {
      family_resource_globals.group_tables[group_index] = 0;
      family_resource_globals.resident_family_ids[group_index] = -1;
      family_resource_globals.group_locks[group_index] = 0;
      family_resource_generations[group_index] = 0;
      family_resource_loaded_bytes[group_index] = 0U;
      group_index++;
    }
  family_resource_globals.group_count = 0;
  family_resource_globals.table_bytes = 0;
  release_family_resource_render_barrier();
dispose_barrier:
  result = dispose_family_resource_render_barrier();
  if(result < 0)
    {
      return result;
    }

  if(family_resource_globals.resource_file_item > 0)
    {
      RSRC_DisposeFileResources(
        family_resource_globals.resource_file_item);
    }
  family_resource_globals.resource_file_item = 0;
  return 0;
}

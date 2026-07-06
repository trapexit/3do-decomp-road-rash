#ifndef ROADRASH_FAMILY_RESOURCE_WORKER_HOST_TEST
  #include "kernel.h"
  #include "task.h"
#endif

#include "family_resource_runtime.h"
#include "rw_semantic_data.h"
#include "thread_launch.h"

enum FamilyResourceWorkerConstant
{
  FAMILY_RESOURCE_WORKER_STACK_BYTES = 0x2EE0,
  FAMILY_RESOURCE_WORKER_PRIORITY = 0x32
};

int
family_resource_worker_main(void);

int
initialize_family_resource_runtime(RoadFamilyScheduleResource *schedule_resource)
{
  int received_signals;
  int result;
  int startup_signal;

  if(initialize_family_resource_render_barrier() == 0)
    {
      return 0;
    }

  result = initialize_family_resource_groups(schedule_resource);
  if(result <= 0)
    {
      shutdown_family_resource_runtime();
      return 0;
    }

  family_resource_globals.wake_signal = 0;
  family_resource_globals.worker_running = 0;
  startup_signal = AllocSignal(0);
  if(startup_signal <= 0)
    {
      family_resource_worker_startup_status =
        FAMILY_RESOURCE_WORKER_FAILED;
      shutdown_family_resource_runtime();
      return 0;
    }
  family_resource_worker_startup_task_item = CURRENTTASK->t.n_Item;
  family_resource_worker_startup_signal = startup_signal;
  family_resource_worker_startup_status =
    FAMILY_RESOURCE_WORKER_STARTING;
  result = launch_named_thread((ThreadEntryPoint)family_resource_worker_main,
                               FAMILY_RESOURCE_WORKER_STACK_BYTES,
                               FAMILY_RESOURCE_WORKER_PRIORITY, "Fido");
  family_resource_globals.worker_task_item = result;
  if(result <= 0)
    {
      family_resource_worker_startup_signal = 0;
      family_resource_worker_startup_task_item = 0;
      FreeSignal(startup_signal);
      family_resource_worker_startup_status =
        FAMILY_RESOURCE_WORKER_FAILED;
      shutdown_family_resource_runtime();
      return 0;
    }

  received_signals = WaitSignal(startup_signal);
  family_resource_worker_startup_signal = 0;
  family_resource_worker_startup_task_item = 0;
  FreeSignal(startup_signal);
  if(received_signals < 0 ||
     (received_signals & startup_signal) == 0 ||
     family_resource_worker_startup_status !=
     FAMILY_RESOURCE_WORKER_READY)
    {
      shutdown_family_resource_runtime();
      return 0;
    }

  return 1;
}

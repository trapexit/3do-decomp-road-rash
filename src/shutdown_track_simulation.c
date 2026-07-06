#include "platform.h"
#include "task.h"

#include "rw_semantic_data.h"

void
dispose_track_simulation_resources(void);

void
shutdown_track_simulation(void)
{
  int wake_signal;
  int worker_task_item;

  track_simulation_globals.worker_running = 0;
  worker_task_item = track_simulation_globals.worker_task_item;
  wake_signal = track_simulation_globals.update_signal |
                track_simulation_globals.completion_signal;
  if(worker_task_item > 0 && wake_signal != 0)
    {
      SendSignal(worker_task_item, wake_signal);
    }
  while(worker_task_item > 0 &&
        CheckItem(worker_task_item, KERNELNODE, TASKNODE) != 0)
    {
      Yield();
    }
  track_simulation_globals.worker_task_item = 0;
  track_simulation_globals.update_signal = 0;
  track_simulation_globals.completion_signal = 0;
  dispose_track_simulation_resources();
}

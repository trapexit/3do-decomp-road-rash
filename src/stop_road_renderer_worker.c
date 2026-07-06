#include "platform.h"
#include "task.h"

#include "memory_manager_runtime.h"
#include "road_renderer_runtime.h"
#include "rw_semantic_data.h"

int
stop_road_renderer_worker(void)
{
  int result;
  int wake_signal;
  int worker_task_item;

  road_renderer_worker_shutdown_requested = 1;
  road_renderer_state.worker_running = 0;
  worker_task_item = road_renderer_state.worker_task_item;
  wake_signal = road_renderer_state.wake_signal;
  result = 0;
  if(worker_task_item > 0 && wake_signal != 0)
    {
      result = SendSignal(worker_task_item, wake_signal);
    }
  while(worker_task_item > 0 &&
        CheckItem(worker_task_item, KERNELNODE, TASKNODE) != 0)
    {
      Yield();
    }
  if(road_renderer_state.reciprocal_table != 0)
    {
      MEM_DisposePointer((char *)road_renderer_state.reciprocal_table);
      road_renderer_state.reciprocal_table = 0;
    }
  road_renderer_state.worker_task_item = 0;
  road_renderer_state.wake_signal = 0;
  road_renderer_worker_shutdown_requested = 0;
  return result;
}

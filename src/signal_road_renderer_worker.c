#include "task.h"

#include "road_renderer_runtime.h"
#include "rw_semantic_data.h"

int
signal_road_renderer_worker(void)
{
  return SendSignal(road_renderer_state.worker_task_item,
                    road_renderer_state.wake_signal);
}

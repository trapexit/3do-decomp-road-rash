/* Reconstructed from the original function at 0x0001ACD0. */



#include "task.h"

#include "rw_semantic_data.h"

int
request_track_simulation_update(void)
{
  int result;

  result = SendSignal(track_simulation_globals.worker_task_item,
                      track_simulation_globals.update_signal);
  if(result < 0)
    {
      return result;
    }
  return track_simulation_globals.worker_running;
}

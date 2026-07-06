#include "task.h"

#include "rw_semantic_data.h"

int
request_track_simulation_completion(unsigned int control_bits,
                                    unsigned int render_control)
{
  int result;

  track_simulation_globals.control_bits = control_bits;
  road_renderer_state.control_flags = render_control;
  result = SendSignal(track_simulation_globals.worker_task_item,
                      track_simulation_globals.completion_signal);
  if(result < 0)
    {
      return result;
    }
  return track_simulation_globals.worker_running;
}

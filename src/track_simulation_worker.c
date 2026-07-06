/* Reconstructed from the original function at 0x0001AD08. */



#include "task.h"

#include "racer_runtime.h"
#include "rw_semantic_data.h"

void
advance_track_simulation_tick(void);
void
update_race_object_schedule(int use_visibility_bands);
void
update_road_view_motion(int current_tick);
int
snapshot_visible_race_objects(void);
int
signal_road_renderer_worker(void);

extern volatile int track_simulation_startup_task_item;
extern volatile int track_simulation_startup_signal;

static void
notify_track_simulation_parent(void)
{
  int task_item;
  int signal;

  task_item = track_simulation_startup_task_item;
  signal = track_simulation_startup_signal;
  if(task_item > 0 && signal != 0)
    {
      SendSignal(task_item, signal);
    }
}

int
signal_track_simulation_worker(void)
{
  int result;

  result = SendSignal(track_simulation_globals.worker_task_item,
                      track_simulation_globals.update_signal |
                      track_simulation_globals.completion_signal);
  if(result < 0)
    {
      return result;
    }
  return track_simulation_globals.worker_running;
}


void
track_simulation_worker_main(void)
{
  int signals;
  int mask;
  int old_node;
  RacerEntity *player;
  track_simulation_globals.update_signal = AllocSignal(0);
  track_simulation_globals.completion_signal = AllocSignal(0);
  if(track_simulation_globals.update_signal <= 0 ||
     track_simulation_globals.completion_signal <= 0 ||
     track_simulation_globals.worker_running == 0)
    {
      track_simulation_globals.worker_running = 0;
      notify_track_simulation_parent();
      goto finish;
    }

  track_simulation_globals.worker_running = 1;
  notify_track_simulation_parent();

  for(;;)
    {
      mask = track_simulation_globals.update_signal |
             track_simulation_globals.completion_signal;
      signals = WaitSignal(mask);
      if(signals < 0 || (signals & SIGF_ABORT) != 0)
        {
          break;
        }
      signals &= mask;
      if(signals != 0)
        {
          if(track_simulation_globals.worker_running == 0)
            {
              break;
            }

          if((signals & track_simulation_globals.update_signal) != 0)
            {
              frame_tick++;
              advance_track_simulation_tick();
            }

          if((signals & track_simulation_globals.completion_signal) != 0)
            {
              if(track_simulation_globals.simulation_active.value != 0)
                {
                  player = (RacerEntity *)race_rider_state.player_object;
                  old_node = player->paired_state->last_update_tick;
                  update_race_object_schedule(track_simulation_globals.enabled);
                  if(old_node != player->paired_state->last_update_tick)
                    {
                      update_road_view_motion((int)frame_tick);
                    }
                }

              road_renderer_state.motion =
                track_simulation_globals.motion_source;
              snapshot_visible_race_objects();
            }

          if(input_thread_state.deferred_event_pending == 0)
            {
              signal_road_renderer_worker();
            }
        }

      if(track_simulation_globals.worker_running == 0)
        {
          break;
        }
    }

finish:
  track_simulation_globals.worker_running = 0;
  if(track_simulation_globals.update_signal > 0)
    {
      FreeSignal(track_simulation_globals.update_signal);
    }
  if(track_simulation_globals.completion_signal > 0)
    {
      FreeSignal(track_simulation_globals.completion_signal);
    }
  track_simulation_globals.completion_signal = 0;
  track_simulation_globals.update_signal = 0;
}

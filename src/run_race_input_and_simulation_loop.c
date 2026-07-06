#include "io.h"

#include "graphics.h"
/* Main-thread race input and simulation coordination. */

#include "item.h"
#include "platform.h"

#include "race_audio_runtime.h"
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"
void
schedule_race_outcome_event(int event,
                            int delay_ticks);
int
select_text_font(int font_item);
int
poll_stream_input_chord(InputChordState *chord);
int
pause_race_audio(void);
int
resume_race_score(void);
void
resolve_race_pause_input(void);
int
signal_track_simulation_worker(void);
int
signal_road_renderer_worker(void);
int
request_track_simulation_update(void);
void
finalize_racer_event_schedule(int current_tick);

#define RACE_INPUT_TASK_PRIORITY          0x34
#define NORMAL_GAME_TASK_PRIORITY         0x31
#define RACE_OVERLAY_WARMUP_FRAMES        0xB4
#define RACE_FORFEIT_INPUT_MASK           0x0F800000U
#define HUD_LAYOUT_TOGGLE_INPUT           0x00800000U
#define RACE_FORFEIT_OUTCOME_EVENT        5
#define INPUT_CHORD_POLL_INTERVAL_MASK    3

static int
hud_layout_toggle_was_pressed(unsigned int current_input,
                              unsigned int previous_input)
{
  return (current_input & HUD_LAYOUT_TOGGLE_INPUT) != 0 &&
         (previous_input & HUD_LAYOUT_TOGGLE_INPUT) == 0;
}

int
run_race_input_and_simulation_loop(void)
{
  InputThreadState *state;
  unsigned int *input_state;
  unsigned int previous_input;
  unsigned int current_input;
  int *ticks;
  unsigned char *event_pending;
  unsigned char *toggle;
  int remaining;
  int result;
  int cleanup_result;

  state = &input_thread_state;
  ticks = (int *)(int)&(frame_tick);
  input_state = (unsigned int *)(int)&(track_simulation_globals.control_bits);
  event_pending = &race_input_latch_state.event_latched;
  toggle = &runtime_feature_flags.alternate_hud_layout.value;

  state->worker_running = 0;
  state->polling_enabled = 0;
  state->wake_signal = 0;
  state->task_item = CURRENTTASK->t.n_Item;
  state->vbl_io_request = GetVBLIOReq();
  if(state->vbl_io_request < 0)
    {
      return state->vbl_io_request;
    }
  state->wake_signal = AllocSignal(0);
  if(state->wake_signal <= 0)
    {
      result = state->wake_signal != 0 ? state->wake_signal : -1;
      state->wake_signal = 0;
      goto finish;
    }

  result = SetItemPri(state->task_item, RACE_INPUT_TASK_PRIORITY);
  if(result < 0)
    {
      goto finish;
    }

  state->polling_enabled = 1;
  state->worker_running = 1;
  state->deferred_event_pending = 0;
  state->deferred_event_ticks = 0;
  state->deferred_event_started_at = 0;
  state->overlay_progress = RACE_OVERLAY_WARMUP_FRAMES;
  road_renderer_state.input_latch = 0;
  *input_state = 0;

  for(;;)
    {
      result = WaitVBLDefer(state->vbl_io_request, 1);
      if(result >= 0)
        {
          result = WaitIO(state->vbl_io_request);
        }
      if(result < 0)
        {
          break;
        }

      if(state->deferred_event_ticks == 0)
        {
          if(state->deferred_event_pending != 0)
            {
              state->worker_running = 0;
            }
        }
      else
        {
          if(state->deferred_event_started_at != 0)
            {
              remaining = state->deferred_event_ticks -
                          (*ticks - state->deferred_event_started_at);
              state->deferred_event_ticks = remaining;
              if(remaining <= 0)
                {
                  state->deferred_event_started_at = 0;
                  state->deferred_event_ticks = 0;
                  state->deferred_event_pending = 1;
                  state->worker_running = 0;
                }
              else if(state->deferred_event_pending == 0)
                {
                  state->deferred_event_started_at = *ticks;
                }
              else
                {
                  state->worker_running = 0;
                }
            }
          else if(state->deferred_event_pending == 0)
            {
              state->deferred_event_started_at = *ticks;
            }
          else
            {
              state->worker_running = 0;
            }
        }

      if(state->polling_enabled != 0)
        {
          if((*ticks & 1) != 0)
            {
              sample_stream_input_pads();
            }

          previous_input = *input_state;
          current_input = get_stream_input_buttons(1);
          *input_state = current_input;

          if(*event_pending != 0 &&
             (current_input & RACE_FORFEIT_INPUT_MASK) != 0)
            {
              state->deferred_event_started_at = 0;
              state->deferred_event_ticks = 0;
              schedule_race_outcome_event(RACE_FORFEIT_OUTCOME_EVENT, 0);
              state->deferred_event_pending = 1;
              *event_pending = 0;
            }

          if(hud_layout_toggle_was_pressed(current_input, previous_input) &&
             state->deferred_event_pending == 0)
            {
              *toggle = (unsigned char)(1 - *toggle);
              if(*toggle == 0)
                {
                  select_text_font(resource_catalog_handles.small_digit_font);
                }
              else
                {
                  select_text_font(resource_catalog_handles.large_digit_font);
                }
            }

          if((*ticks & INPUT_CHORD_POLL_INTERVAL_MASK) == 0)
            {
              result = poll_stream_input_chord(&state->primary_chord);
              state->chord_processing_active = (result == 0);
              if(result == 0)
                {
                  resolve_race_pause_input();
                  if(state->worker_running == 0)
                    {
                      result = -1;
                      break;
                    }
                }
            }

          if(state->deferred_event_pending == 0)
            {
              if(state->overlay_progress != 0)
                {
                  if(*ticks == 0)
                    {
                      result = signal_track_simulation_worker();
                      if(result <= 0)
                        {
                          result = -1;
                          break;
                        }
                    }
                  else
                    {
                      update_engine_audio_modulation();
                      result = signal_road_renderer_worker();
                      if(result < 0)
                        {
                          break;
                        }
                      state->overlay_progress--;
                    }
                }
              else
                {
                  result = request_track_simulation_update();
                  if(result <= 0)
                    {
                      result = -1;
                      break;
                    }
                  result = WaitSignal(state->wake_signal);
                  if(result < 0 || (result & SIGF_ABORT) != 0)
                    {
                      result = -1;
                      break;
                    }
                  if((result & state->wake_signal) != 0)
                    {
                      service_race_audio_events();
                    }
                }
            }
        }

      if(state->worker_running == 0)
        {
          finalize_racer_event_schedule(*ticks);
          result = 0;
          break;
        }
    }

finish:
  state->worker_running = 0;
  state->polling_enabled = 0;
  SetItemPri(state->task_item, NORMAL_GAME_TASK_PRIORITY);
  if(state->wake_signal != 0)
    {
      FreeSignal(state->wake_signal);
      state->wake_signal = 0;
    }
  cleanup_result = DeleteItem(state->vbl_io_request);
  state->vbl_io_request = 0;
  return result < 0 ? result : cleanup_result;
}


void
resolve_race_pause_input(void)
{
  InputThreadState *state;
  int ready;
  int result;

  pause_race_audio();
  state = &input_thread_state;

  for(;;)
    {
      signal_road_renderer_worker();
      result = WaitVBLDefer(state->vbl_io_request, 1);
      if(result >= 0)
        {
          result = WaitIO(state->vbl_io_request);
        }
      if(result < 0)
        {
          state->worker_running = 0;
          state->chord_processing_active = 0;
          return;
        }
      sample_stream_input_pads();

      ready = poll_stream_input_chord(&state->alternate_chord) == 0;
      if(poll_stream_input_chord(&state->primary_chord) == 0 || ready)
        {
          break;
        }
    }

  if(ready)
    {
      schedule_race_outcome_event(RACE_FORFEIT_OUTCOME_EVENT, 0);
    }
  else
    {
      resume_race_score();
    }
  state->chord_processing_active = 0;
}

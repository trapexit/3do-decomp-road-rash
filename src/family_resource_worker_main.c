#ifndef ROADRASH_FAMILY_RESOURCE_WORKER_HOST_TEST
  #include "audio.h"
  #include "task.h"
#else
int
AllocSignal(int requested_signal);
int
FreeSignal(int signal_mask);
int
WaitSignal(int signal_mask);
int
SendSignal(int task_item,
           int signal_mask);
int
OpenAudioFolio(void);
int
CloseAudioFolio(void);
#endif

#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

static
void
notify_family_resource_worker_parent(void)
{
  int startup_signal;
  int startup_task_item;

  startup_signal = family_resource_worker_startup_signal;
  startup_task_item = family_resource_worker_startup_task_item;
  if(startup_task_item > 0 && startup_signal != 0)
    {
      SendSignal(startup_task_item, startup_signal);
    }
}


int
family_resource_worker_main(void)
{
  int signal;
  int signal_mask;
  int group_index;
  int requested_id;
  int current_id;
  int mutation_required;
  void *load_result;
  FamilyResourceWorkerRequest request;

  signal = AllocSignal(0);
  if(signal <= 0)
    {
      family_resource_globals.wake_signal = 0;
      family_resource_globals.worker_running = 0;
      family_resource_worker_startup_status =
        FAMILY_RESOURCE_WORKER_FAILED;
      notify_family_resource_worker_parent();
      return signal;
    }

  family_resource_globals.wake_signal = signal;
  if(family_resource_worker_shutdown_requested != 0)
    {
      signal = FreeSignal(family_resource_globals.wake_signal);
      family_resource_globals.wake_signal = 0;
      family_resource_globals.worker_running = 0;
      family_resource_worker_startup_status =
        FAMILY_RESOURCE_WORKER_NOT_READY;
      notify_family_resource_worker_parent();
      return signal;
    }
  signal = OpenAudioFolio();
  if(signal < 0)
    {
      FreeSignal(family_resource_globals.wake_signal);
      family_resource_globals.wake_signal = 0;
      family_resource_globals.worker_running = 0;
      family_resource_worker_startup_status =
        FAMILY_RESOURCE_WORKER_FAILED;
      notify_family_resource_worker_parent();
      return signal;
    }
  family_resource_globals.worker_running = 1;
  if(family_resource_worker_shutdown_requested != 0)
    {
      CloseAudioFolio();
      signal = FreeSignal(family_resource_globals.wake_signal);
      family_resource_globals.wake_signal = 0;
      family_resource_globals.worker_running = 0;
      family_resource_worker_startup_status =
        FAMILY_RESOURCE_WORKER_NOT_READY;
      notify_family_resource_worker_parent();
      return signal;
    }
  family_resource_worker_startup_status =
    FAMILY_RESOURCE_WORKER_READY;
  notify_family_resource_worker_parent();

  for(;;)
    {
      if(family_resource_worker_shutdown_requested != 0 ||
         family_resource_globals.worker_running == 0)
        {
          break;
        }
      signal_mask = family_resource_globals.wake_signal;
      signal = WaitSignal(signal_mask);
      if(signal < 0)
        {
          break;
        }
      if((signal & family_resource_globals.wake_signal) != 0)
        {
          if(family_resource_worker_shutdown_requested != 0 ||
             family_resource_globals.worker_running == 0)
            {
              break;
            }

          if(family_resource_worker_request_pending != 0)
            {
              request = *(FamilyResourceWorkerRequest *)
                        family_resource_globals.worker_request;
              group_index = request.group_index;
              current_id = family_resource_globals
                           .resident_family_ids[group_index];
              requested_id = request.family_id;
              mutation_required = 0;

              if(current_id != requested_id &&
                 current_id != -requested_id)
                {
                  seek_family_resource(&request);
                  if(family_resource_globals.last_seek_result != 0)
                    {
                      mutation_required = 1;
                    }
                  else if(request.mode ==
                          FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
                    {
                      mutation_required = 1;
                    }
                }
              else if(request.mode ==
                      FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
                {
                  mutation_required = 1;
                }

              if(mutation_required != 0)
                {
                  if(acquire_family_resource_render_barrier() == 0)
                    {
                      if(request.mode == FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
                        {
                          release_family_resource_lock(
                            &family_resource_globals.group_locks[group_index]);
                        }
                      family_resource_worker_request_pending = 0;
                      break;
                    }
                  if(family_resource_globals.worker_running != 0 &&
                     current_id != requested_id &&
                     current_id != -requested_id)
                    {
                      if(family_resource_globals.last_seek_result != 0)
                        {
                          mark_family_resource_group_unavailable(
                            request.group_index, request.family_id);
                        }
                      else if(request.mode ==
                              FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
                        {
                          load_result =
                            load_and_fixup_family_resource_group(
                              &request);
                          if(load_result == 0)
                            {
                              mark_family_resource_group_unavailable(
                                request.group_index,
                                request.family_id);
                            }
                        }
                    }

                  if(request.mode ==
                     FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE)
                    {
                      release_family_resource_lock(
                        &family_resource_globals
                        .group_locks[group_index]);
                    }
                  family_resource_worker_request_pending = 0;
                  release_family_resource_render_barrier();
                }
              else
                {
                  family_resource_worker_request_pending = 0;
                }
            }
        }

      if(family_resource_worker_shutdown_requested != 0 ||
         family_resource_globals.worker_running == 0)
        {
          break;
        }
    }

  CloseAudioFolio();
  signal = FreeSignal(family_resource_globals.wake_signal);
  family_resource_globals.wake_signal = 0;
  family_resource_globals.worker_running = 0;
  family_resource_worker_startup_status =
    FAMILY_RESOURCE_WORKER_NOT_READY;
  return signal;
}

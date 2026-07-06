#include "msgport.h"

#include "audio.h"
/* Reconstructed from the original function at 0x000312B8. */



#include "platform.h"

#include "async_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "rw_semantic_data.h"

int
stop_async_stream(AsyncStreamPlaybackStatePrefix *state,
                  int                            *stop_accepted)
{
  Item message_item;
  void *thread_stack_block;
  Item playback_thread_item;
  int wait_result;
  int result;

  if(stop_accepted != 0)
    {
      *stop_accepted = 0;
    }

  if(state == 0)
    {
      return 0;
    }

  message_item = state->command_message_item;
  thread_stack_block = state->thread_stack_block;
  playback_thread_item = state->playback_thread_item;

  /* The worker frees state before replying.  A successfully queued stop
   * therefore transfers state ownership even when the reply wait fails;
   * only these pre-send snapshots remain safe for caller-side cleanup. */
  result = SendSmallMsg(state->command_port_item, message_item,
                        ASYNC_STREAM_COMMAND_STOP, 0);
  if(result < 0)
    {
      return result;
    }
  if(stop_accepted != 0)
    {
      *stop_accepted = 1;
    }

  wait_result = WaitPort(async_stream_message_port, message_item);
  if(wait_result >= 0)
    {
      DeleteItem(wait_result);
    }

  while(CheckItem(playback_thread_item, KERNELNODE, TASKNODE) != 0)
    {
      SleepAudioTicks(ASYNC_STREAM_THREAD_POLL_TICKS);
    }

  if(wait_result < 0)
    {
      DeleteItem(message_item);
    }
  dispose_data_stream_thread_stack(thread_stack_block);
  return wait_result < 0 ? wait_result : 0;
}

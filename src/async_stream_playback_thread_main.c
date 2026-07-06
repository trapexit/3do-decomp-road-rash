#include "msgport.h"

#include "audio.h"
#include "operror.h"
#include "stdio.h"
#include "stdlib.h"

#include "platform.h"

#include "async_stream_runtime.h"

typedef void (*RwMemoryDeallocator)(void *pointer,
                                    int   byte_count);

void
BF_GetHooks(int *allocate,
            int *deallocate);
static
Item
current_task_item(void)
{
  return CURRENTTASK->t.n_Item;
}


/*
 * Reconstructed from the thread body at 0x00031FD4 and the tail of the
 * original function at 0x00031E3C.
 */
void
async_stream_playback_thread_main(int  unused_initial_argument,
                                  Item parent_port_item)
{
  char name[ASYNC_STREAM_OBJECT_NAME_BYTES];
  unsigned char paused;
  volatile int running;
  int allocate;
  int deallocate;
  Item command_port_item;
  Item startup_message_item;
  Item playback_semaphore_item;
  Item buffering_semaphore_item;
  Item command_message_item;
  int wait_result;
  int dispatch_result;
  int result;
  int audio_open;
  Message *message;
  AsyncStreamPlaybackStatePrefix *state;
  const AsyncStreamOpenRequest *request;
  RwMemoryDeallocator free_state;

  (void)unused_initial_argument;
  paused = 0;
  command_port_item = 0;
  startup_message_item = 0;
  playback_semaphore_item = 0;
  buffering_semaphore_item = 0;
  command_message_item = 0;
  state = 0;
  audio_open = 0;
  BF_GetHooks(&allocate, &deallocate);

  sprintf(name, "%s(%ld)", "AS_MsgPort",
          current_task_item());
  command_port_item = CreateMsgPort(name, 0, 0);
  if(command_port_item < 0)
    {
      goto cleanup;
    }

  sprintf(name, "%s(%ld)", "AS_StreamMsg",
          current_task_item());
  startup_message_item = CreateSmallMsg(
    name, ASYNC_STREAM_MESSAGE_PRIORITY, command_port_item);
  if(startup_message_item < 0)
    {
      goto cleanup;
    }

  sprintf(name, "%s(%ld)", "AS_Sema",
          current_task_item());
  playback_semaphore_item = CreateSemaphore(name, 0);
  if(playback_semaphore_item < 0)
    {
      goto cleanup;
    }

  sprintf(name, "asyncSema(%ld)", current_task_item());
  buffering_semaphore_item = CreateSemaphore(name, 0);
  if(buffering_semaphore_item < 0)
    {
      goto cleanup;
    }

  result = SendMsg(parent_port_item, startup_message_item, 0, 0);
  if(result < 0)
    {
      goto cleanup;
    }

  wait_result = WaitPort(command_port_item, startup_message_item);
  if(wait_result < 0)
    {
      goto cleanup;
    }

  message = (Message *)LookupItem(startup_message_item);
  if(message == 0)
    {
      goto cleanup;
    }
  command_message_item = (Item)message->msg_DataPtr;

  wait_result = WaitPort(command_port_item, command_message_item);
  if(wait_result < 0)
    {
      goto cleanup;
    }

  message = (Message *)LookupItem(command_message_item);
  if(message == 0)
    {
      goto cleanup;
    }
  request = (const AsyncStreamOpenRequest *)message->msg_DataSize;

  result = OpenAudioFolio();
  if(result < 0)
    {
      ReplyMsg(command_message_item,
               ASYNC_STREAM_COMMAND_STARTUP, 0, 0);
      goto cleanup;
    }
  audio_open = 1;

  state = create_async_stream_playback_state(
    (RwMemoryAllocator)allocate, request);
  if(state == 0)
    {
      result = ReplyMsg(command_message_item,
                        ASYNC_STREAM_COMMAND_STARTUP, 0, 0);
      if(result < 0)
        {
          goto cleanup;
        }
      goto cleanup;
    }

  state->command_port_item = command_port_item;
  state->command_message_item = command_message_item;
  state->playback_semaphore_item = playback_semaphore_item;
  state->buffering_semaphore_item = buffering_semaphore_item;
  state->buffering_state = ASYNC_STREAM_BUFFERING_ENABLED;
  state->requested_audio_amplitude = MAXDSPAMPLITUDE;
  state->applied_audio_amplitude = 0;

  result = ReplyMsg(command_message_item,
                    ASYNC_STREAM_COMMAND_STARTUP, state, 0);
  if(result < 0)
    {
      goto cleanup;
    }


  running = 1;
  do
    {
      wait_result = WaitPort(command_port_item, 0);
      if(wait_result < 0)
        {
          state->block_stream->last_error = wait_result;
          dispatch_result = ASYNC_STREAM_DISPATCH_CONTINUE;
          continue;
        }
      dispatch_result = dispatch_async_stream_command(
        state, wait_result, 0, &paused);
    } while(dispatch_result != ASYNC_STREAM_DISPATCH_STOP && running != 0);

  dispose_async_stream_playback_resources(state);
  free_state = (RwMemoryDeallocator)deallocate;
  free_state(state, ASYNC_STREAM_PLAYBACK_STATE_BYTES);
  state = 0;
  ReplyMsg(command_message_item, ASYNC_STREAM_COMMAND_STOP, 0, 0);

cleanup:
  if(state != 0)
    {
      dispose_async_stream_playback_resources(state);
      free_state = (RwMemoryDeallocator)deallocate;
      free_state(state, ASYNC_STREAM_PLAYBACK_STATE_BYTES);
    }
  if(audio_open != 0)
    {
      CloseAudioFolio();
    }
  if(buffering_semaphore_item > 0)
    {
      DeleteItem(buffering_semaphore_item);
    }
  if(playback_semaphore_item > 0)
    {
      DeleteItem(playback_semaphore_item);
    }
  if(startup_message_item > 0)
    {
      DeleteItem(startup_message_item);
    }
  if(command_port_item > 0)
    {
      DeleteItem(command_port_item);
    }
  exit(0);
}

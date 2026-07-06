#include "msgport.h"

#include "operror.h"

#include "platform.h"

#include "async_stream_runtime.h"

int
set_stream_audio_playback_amplitude(int amplitude);

/* Reconstructed from the original function at 0x00031700. */
int
service_async_stream_messages(AsyncStreamPlaybackStatePrefix *state,
                              unsigned char                  *paused,
                              unsigned char                  *continue_playback,
                              int                             yield_when_idle,
                              int                             refresh_audio_amplitude)
{
  Item message_item;
  int dispatch_status;
  int result;

  yield_when_idle = (unsigned char)yield_when_idle;
  refresh_audio_amplitude = (unsigned char)refresh_audio_amplitude;

  result = LockItem(state->buffering_semaphore_item, 1);
  if(result < 0)
    {
      *continue_playback = 0;
      return result;
    }

  switch(state->buffering_state)
    {
    case ASYNC_STREAM_BUFFERING_ENABLE_PENDING:
      result = set_async_block_stream_buffering(
        state->block_stream, 1, 0);
      if(result >= 0)
        {
          state->buffering_state = ASYNC_STREAM_BUFFERING_ENABLED;
        }
      break;

    case ASYNC_STREAM_BUFFERING_DISABLE_PENDING:
      result = set_async_block_stream_buffering(
        state->block_stream, 0, 0);
      if(result >= 0)
        {
          state->buffering_state = ASYNC_STREAM_BUFFERING_DISABLED;
        }
      break;

    default:
      break;
    }

  UnlockItem(state->buffering_semaphore_item);
  if(result < 0)
    {
      *continue_playback = 0;
      return result;
    }

  if(refresh_audio_amplitude != 0 &&
     state->requested_audio_amplitude != state->applied_audio_amplitude)
    {
      state->applied_audio_amplitude =
        set_stream_audio_playback_amplitude(
          state->requested_audio_amplitude);
    }

  for(;;)
    {
      if(*paused == 0)
        {
          message_item = GetMsg(state->command_port_item);
        }
      else
        {
          message_item = WaitPort(state->command_port_item, 0);
        }

      if(message_item == 0)
        {
          if(yield_when_idle != 0)
            {
              Yield();
            }
          return ASYNC_STREAM_DISPATCH_CONTINUE;
        }
      if(message_item < 0)
        {
          *continue_playback = 0;
          return message_item;
        }

      dispatch_status = dispatch_async_stream_command(
        state, message_item, 1, paused);
      if(dispatch_status == ASYNC_STREAM_DISPATCH_STOP)
        {
          *continue_playback = 0;
          return ASYNC_STREAM_DISPATCH_STOP;
        }

      yield_when_idle = 0;
    }
}

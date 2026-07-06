#include "msgport.h"

#include "operror.h"
#include "platform.h"

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

int
set_async_stream_buffering_mode(int  stream_handle,
                                int  buffering_enabled,
                                int  wait_for_change,
                                int *buffer_count_out)
{
  AsyncStreamPlaybackStatePrefix *state;
  Message *message;
  int mode;
  int result;

  buffering_enabled = (unsigned char)buffering_enabled;
  wait_for_change = (unsigned char)wait_for_change;
  if(stream_handle == 0)
    {
      return 0;
    }
  if(buffer_count_out == ASYNC_STREAM_UPDATE_STATE_ONLY &&
     wait_for_change != 0)
    {
      return -1;
    }

  state = (AsyncStreamPlaybackStatePrefix *)stream_handle;
  mode = 0;
  if(buffering_enabled != 0)
    {
      mode |= ASYNC_STREAM_BUFFERING_FLAG_ENABLE;
    }
  if(wait_for_change != 0)
    {
      mode |= ASYNC_STREAM_BUFFERING_FLAG_WAIT;
    }

  if(buffer_count_out == ASYNC_STREAM_UPDATE_STATE_ONLY &&
     wait_for_change == 0)
    {
      result = LockItem(state->buffering_semaphore_item, 1);
      if(result < 0)
        {
          return result;
        }

      switch(state->buffering_state)
        {
        case ASYNC_STREAM_BUFFERING_ENABLED:
          if(buffering_enabled == 0)
            {
              state->buffering_state =
                ASYNC_STREAM_BUFFERING_DISABLE_PENDING;
            }
          break;
        case ASYNC_STREAM_BUFFERING_DISABLED:
          if(buffering_enabled == 1)
            {
              state->buffering_state =
                ASYNC_STREAM_BUFFERING_ENABLE_PENDING;
            }
          break;
        case ASYNC_STREAM_BUFFERING_ENABLE_PENDING:
          if(buffering_enabled == 0)
            {
              state->buffering_state = ASYNC_STREAM_BUFFERING_DISABLED;
            }
          break;
        case ASYNC_STREAM_BUFFERING_DISABLE_PENDING:
          if(buffering_enabled == 1)
            {
              state->buffering_state = ASYNC_STREAM_BUFFERING_ENABLED;
            }
          break;
        }

      result = UnlockItem(state->buffering_semaphore_item);
      if(result < 0)
        {
          return result;
        }
      return 0;
    }

  result = SendSmallMsg(state->command_port_item,
                        state->command_message_item,
                        ASYNC_STREAM_COMMAND_BUFFERING, mode);
  if(result < 0)
    {
      PrintfSysErr(result);
      return result;
    }

  result = WaitPort(async_stream_message_port,
                    state->command_message_item);
  if(result < 0)
    {
      return result;
    }

  message = (Message *)LookupItem(result);
  if(message == 0)
    {
      return -1;
    }

  if(buffer_count_out != 0)
    {
      *buffer_count_out = (int)message->msg_DataPtr;
    }

  result = message->msg_DataSize;
  if(result != 0)
    {
      result = -result;
    }
  return result;
}

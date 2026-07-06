#include "audio.h"
#include "item.h"

#include "async_stream_runtime.h"

/* Reconstructed from the dispatcher prefix of the original function at 0x00031E3C. */
int
dispatch_async_stream_command(AsyncStreamPlaybackStatePrefix *state,
                              Item                            message_item,
                              int                             from_playback_loop,
                              unsigned char                  *paused)
{
  Message *message;
  int command;
  int reply_value;
  int reply_data_size;
  int result;
  int flags;

  message = (Message *)LookupItem(message_item);
  if(message == 0)
    {
      return ASYNC_STREAM_DISPATCH_INVALID_MESSAGE;
    }

  command = (int)message->msg_DataPtr;
  reply_value = 0;
  reply_data_size = 0;
  result = ASYNC_STREAM_DISPATCH_CONTINUE;

  switch(command)
    {
    case ASYNC_STREAM_COMMAND_STARTUP:
      reply_value = -1;
      break;

    case ASYNC_STREAM_COMMAND_PLAY:
      if((unsigned char)from_playback_loop != 0)
        {
          if(*paused == 0)
            {
              reply_value = -1;
            }
          else
            {
              *paused = 0;
            }
          break;
        }

      reply_value = LockItem(state->playback_semaphore_item, 0);
      if(reply_value <= 0)
        {
          if(reply_value == 0)
            {
              reply_value = -1;
            }
          break;
        }
      if(state->block_stream != 0)
        {
          state->block_stream->last_error = 0;
        }
      state->active = 1;

      merge_stream_playback_tag_overrides(
        state,
        (const RwTagArgument *)message->msg_DataSize);
      result = ReplyMsg(message_item, command, 0, 0);
      if(result < 0)
        {
          state->active = 0;
          UnlockItem(state->playback_semaphore_item);
          return result;
        }
      result = run_async_stream_playback(state);
      if(result < 0 && state->block_stream != 0)
        {
          state->block_stream->last_error = result;
        }
      state->active = 0;
      UnlockItem(state->playback_semaphore_item);
      return result;

    case ASYNC_STREAM_COMMAND_CONTROL:
      state->control_value = message->msg_DataSize;
      break;

    case ASYNC_STREAM_COMMAND_PAUSE:
      *paused = 1;
      break;

    case ASYNC_STREAM_COMMAND_REPEAT:
      state->repeat = (unsigned char)message->msg_DataSize;
      break;

    case ASYNC_STREAM_COMMAND_BUFFERING:
      flags = message->msg_DataSize;
      result = set_async_block_stream_buffering(
        state->block_stream,
        (flags & ASYNC_STREAM_BUFFERING_FLAG_ENABLE) != 0,
        (flags & ASYNC_STREAM_BUFFERING_FLAG_WAIT) != 0);
      if(result < 0)
        {
          reply_data_size = -result;
        }
      else
        {
          reply_value = state->buffer_group_size * result;
        }
      break;

    case ASYNC_STREAM_COMMAND_STOP:
      return ASYNC_STREAM_DISPATCH_STOP;

    default:
      break;
    }

  ReplyMsg(message_item, command, (void *)reply_value, reply_data_size);
  return ASYNC_STREAM_DISPATCH_CONTINUE;
}

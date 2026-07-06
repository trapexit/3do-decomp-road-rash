#include "msgport.h"


#include "platform.h"

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

int
start_async_stream_playback(AsyncStreamPlaybackStatePrefix *state,
                            const RwTagArgument            *tags)
{
  int send_result;
  int wait_result;
  Message *message;

  if(state == 0)
    {
      return 0;
    }

  send_result = SendSmallMsg(state->command_port_item,
                             state->command_message_item,
                             ASYNC_STREAM_COMMAND_PLAY,
                             (uint32)tags);
  if(send_result < 0)
    {
      return send_result;
    }

  wait_result = WaitPort(async_stream_message_port,
                         state->command_message_item);
  if(wait_result >= 0)
    {
      message = (Message *)LookupItem(wait_result);
      return message != 0 ? (int)message->msg_DataPtr : -1;
    }
  return wait_result;
}

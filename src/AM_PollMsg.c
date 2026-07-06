#include "audio_message_runtime.h"

int32
AM_PollMsg(unsigned char *out_received,
           Item           message_port_item,
           Item          *out_message_item,
           int32         *out_message_value,
           int32         *out_message_value_size)
{
  Message *message;
  int32 poll_status;
  int32 received;
  int32 result;

  message = 0;
  poll_status = 0;
  result = 0;

  received = PollForMsg(message_port_item, out_message_item,
                        &message, 0, &poll_status);
  if(out_received != 0)
    {
      *out_received = (unsigned char)received;
    }

  if(received != 0)
    {
      if(message != 0)
        {
          if(out_message_value != 0)
            {
              *out_message_value = (int32)message->msg_DataPtr;
            }
          if(out_message_value_size != 0)
            {
              *out_message_value_size = message->msg_DataSize;
            }
        }
    }
  else if(poll_status >= -1)
    {
      result = poll_status;
    }

  return result;
}

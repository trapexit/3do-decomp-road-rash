#include "audio_message_runtime.h"

int32
AM_WaitMsg(Item   message_port_item,
           Item  *out_message_item,
           int32 *out_message_value,
           int32 *out_message_value_size)
{
  Message *message;
  int32 result;

  message = 0;
  result = WaitForMsg(message_port_item, out_message_item, &message, 0, 0);

  if(result >= 0 && message != 0)
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

  return result;
}

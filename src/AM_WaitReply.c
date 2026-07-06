#include "audio_message_runtime.h"

int32
AM_WaitReply(Item   message_port_item,
             Item   reply_message_item,
             int32 *out_reply_result,
             int   *out_reply_value,
             int32 *out_reply_value_size)
{
  int32 result;
  Message *reply_message;

  reply_message = 0;
  if(LookupItem(reply_message_item) == 0)
    {
      return -1;
    }

  result = WaitForMsg(message_port_item, 0, &reply_message, 0,
                      reply_message_item);
  if(result < 0 || reply_message == 0)
    {
      return result;
    }

  if(out_reply_result != 0)
    {
      *out_reply_result = reply_message->msg_Result;
    }
  if(out_reply_value != 0)
    {
      *out_reply_value = (int)reply_message->msg_DataPtr;
    }
  if(out_reply_value_size != 0)
    {
      *out_reply_value_size = reply_message->msg_DataSize;
    }
  return result;
}

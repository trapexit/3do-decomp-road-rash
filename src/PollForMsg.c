#include "audio_message_runtime.h"

boolean
PollForMsg(Item      message_port_item,
           Item     *out_message_item,
           Message **out_message,
           void    **out_payload,
           int32    *out_status)
{
  Item message_item;
  Message *message;

  message_item = GetMsg(message_port_item);
  if(message_item <= 0)
    {
      *out_status = message_item;
      return 0;
    }

  message = (Message *)LookupItem(message_item);
  if(message == 0)
    {
      *out_status = -1;
      return 0;
    }

  if(out_message_item != 0)
    {
      *out_message_item = message_item;
    }
  if(out_message != 0)
    {
      *out_message = message;
    }
  if(out_payload != 0)
    {
      *out_payload = message->msg_DataPtr;
    }

  *out_status = 0;
  return 1;
}


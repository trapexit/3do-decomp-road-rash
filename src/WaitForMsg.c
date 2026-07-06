#include "msgport.h"

#include "audio_message_runtime.h"

int32
WaitForMsg(Item      message_port_item,
           Item     *out_message_item,
           Message **out_message,
           void    **reserved_payload_output,
           Item      wanted_message_item)
{
  Item message_item;
  Message *message;

  (void)reserved_payload_output;
  message_item = WaitPort(message_port_item, wanted_message_item);
  if(message_item < 0)
    {
      return message_item;
    }

  if(out_message != 0)
    {
      message = (Message *)LookupItem(message_item);
      *out_message = message;
      if(message == 0)
        {
          return -1;
        }
    }
  if(out_message_item != 0)
    {
      *out_message_item = message_item;
    }
  return 0;
}


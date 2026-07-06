#include "audio_message_runtime.h"

uint32
GetMsgPortSignal(Item message_port_item)
{
  MsgPort *message_port;

  message_port = (MsgPort *)LookupItem(message_port_item);
  if(message_port != 0)
    {
      return message_port->mp_Signal;
    }
  return 0;
}

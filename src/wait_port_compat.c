#include "kernel.h"
#include "msgport.h"
#include "operror.h"

#include "wait_port_provider_bridge.h"

/*
 * The Portfolio 1.2 WaitPort kernel vector returns an error on the retail
 * kernel used by Road Rash.  Reproduce its intended semantics with the
 * contemporary GetMsg/GetThisMsg calls and the message port's signal.
 */
Item
_WaitPort(Item port,
          Item message)
{
  MsgPort *message_port;
  Message *wanted_message;
  Item received_message;
  int32 received_signals;

  message_port = (MsgPort *)LookupItem(port);
  if(message_port == 0)
    {
      return BADITEM;
    }
  wanted_message = 0;
  if(message != 0)
    {
      wanted_message = (Message *)LookupItem(message);
      if(wanted_message == 0)
        {
          return BADITEM;
        }
    }

  for(;;)
    {
      if(message == 0)
        {
          received_message = GetMsg(port);
        }
      else if(wanted_message->msg_MsgPort == port)
        {
          received_message = GetThisMsg(message);
        }
      else
        {
          received_message = 0;
        }
      if(received_message != 0)
        {
          return received_message;
        }

      received_signals = WaitSignal(message_port->mp_Signal);
      if(received_signals < 0)
        {
          return received_signals;
        }
    }
}


Item
WaitPort(Item port,
         Item message)
{
  return _WaitPort(port, message);
}

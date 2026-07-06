#include "audio_message_runtime.h"

Item
NewMsgPort(uint32 *out_signal_mask)
{
  Item message_port_item;

  message_port_item = CreateSizedItem(
    MKNODEID(KERNELNODE, MSGPORTNODE), 0, 0);
  if(message_port_item > 0 && out_signal_mask != 0)
    {
      *out_signal_mask = GetMsgPortSignal(message_port_item);
    }

  return message_port_item;
}


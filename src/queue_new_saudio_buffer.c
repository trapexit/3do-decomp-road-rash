#include "msgport.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "subscriberutils.h"

#include "saudio_subscriber_runtime.h"

void
QueueNewAudioBuffer(SAudioContext           *context,
                    SAudioSubscriberMessage *message)
{
  SAudioSampleChunk *sample_chunk;
  SAudioChannel *channel;
  int channel_number;

  sample_chunk = (SAudioSampleChunk *)message->payload.data.buffer;
  channel_number = sample_chunk->channel;
  channel = &context->channels[channel_number];

  if((channel->status & SAUDIO_CHANNEL_ENABLED_FLAG) != 0)
    {
      AddDataMsgToTail(
        (SubsQueuePtr) & channel->pending_messages,
        (SubscriberMsgPtr)message);
      MoveWaitingMsgsToBufferQueue(context, channel_number);
      return;
    }

  ReplyMsg(message->message_item, 0, message,
           sizeof(SAudioSubscriberMessage));
}

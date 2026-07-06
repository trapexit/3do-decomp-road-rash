#include "msgport.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "subscriberutils.h"
/* FlushChannel - reconstructed from the original function at 0x0004B8F4 */

#include "saudio_subscriber_runtime.h"

long
FlushChannel(SAudioContext *context,
             long           channel_number)
{
  SAudioChannel *channel;
  SAudioSubscriberMessage *message;
  SAudioBuffer *buffer;
  long status;
  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number))
    {
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
    }


  /* The legacy disabled-channel path returned an uninitialized value.
   * Define it as success instead of preserving undefined C behavior. */
  status = 0;
  channel = &context->channels[channel_number];

  if((channel->status & SAUDIO_CHANNEL_ENABLED_FLAG) != 0)
    {
      status = StopChannel(context, channel_number);
      if(status < 0)
        {
          return status;
        }

      message = (SAudioSubscriberMessage *)GetNextDataMsg(
        (SubsQueuePtr) & channel->pending_messages);
      while(message != 0)
        {
          status = ReplyMsg(message->message_item, 0, message,
                            sizeof(SAudioSubscriberMessage));
          message = (SAudioSubscriberMessage *)GetNextDataMsg(
            (SubsQueuePtr) & channel->pending_messages);
        }

      buffer = GetNextBuffer(channel);
      while(buffer != 0)
        {
          message = buffer->pending_message;
          buffer->pending_message = 0;
          FreeBufferFromAudioFolio(buffer);
          status = ReplyMsg(message->message_item, 0, message,
                            sizeof(SAudioSubscriberMessage));
          context->all_buffer_signals &= ~buffer->signal;
          ReturnPoolMem(channel->buffer_pool, buffer);
          buffer = GetNextBuffer(channel);
        }
    }

  return status;
}

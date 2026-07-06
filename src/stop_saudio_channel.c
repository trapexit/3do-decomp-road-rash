/* StopChannel - reconstructed from the original function at 0x0004B80C */



#include "audio.h"
#include "platform.h"
#include "saudio_subscriber_runtime.h"

long
StopChannel(SAudioContext *context,
            long           channel_number)
{
  SAudioSubscriberMessage *message;
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  unsigned int pending_signals;
  long result;
  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number))
    {
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
    }


  /* The legacy no-op path returned an uninitialized value.  Define it as
   * success instead of preserving undefined C behavior. */
  result = 0;
  channel = &context->channels[channel_number];

  if((channel->status & SAUDIO_CHANNEL_PLAYING_FLAGS) ==
     SAUDIO_CHANNEL_PLAYING_FLAGS)
    {
      buffer = channel->in_use_queue_head;
      if(buffer != 0)
        {
          MuteChannel(context, channel_number, SAUDIO_INTERNAL_MUTE);
          result = StopAttachment(buffer->attachment, 0);
          if(result < 0)
            {
              return result;
            }
          GetNextBuffer(channel);
          channel->attachments_running = 0;

          pending_signals = GetCurrentSignals();
          if((pending_signals & buffer->signal) != 0)
            {
              WaitSignal(buffer->signal);
            }

          message = buffer->pending_message;
          buffer->pending_message = 0;
          FreeBufferFromAudioFolio(buffer);
          result = ReplyMsg(message->message_item, 0, message,
                            sizeof(SAudioSubscriberMessage));
          context->all_buffer_signals &= ~buffer->signal;
          ReturnPoolMem(channel->buffer_pool, buffer);
        }
      channel->status &= ~SAUDIO_CHANNEL_ACTIVE_FLAG;
    }

  return result;
}

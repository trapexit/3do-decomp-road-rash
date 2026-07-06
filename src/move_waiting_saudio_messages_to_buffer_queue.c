#include "stdio.h"

#include "audio.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "saudio_subscriber_runtime.h"
#include "subscriberutils.h"

void
MoveWaitingMsgsToBufferQueue(SAudioContext *context,
                             int            channel_number)
{
  TagArg tags[SAUDIO_DYNAMIC_BUFFER_TAG_COUNT];
  SAudioSubscriberMessage *message;
  SAudioSampleChunk *sample_chunk;
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  int status;

  channel = &context->channels[channel_number];
  if((channel->status & SAUDIO_CHANNEL_ACTIVE_FLAG) == 0)
    {
      return;
    }

  if(channel->output.instrument <= 0)
    {
      if(channel->pending_messages.head != 0)
        {
          printf(
            "\n\n SERIOUS ERROR on channel %ld! -- Data buffers arrived "
            "on an uninitalized channel!\n\n",
            channel_number);
        }
      return;
    }

  while(channel->pending_messages.head != 0)
    {
      buffer = AllocPoolMem(channel->buffer_pool);
      if(buffer == 0)
        {
          return;
        }

      message = (SAudioSubscriberMessage *)GetNextDataMsg(
        (SubsQueuePtr) & channel->pending_messages);
      sample_chunk = (SAudioSampleChunk *)message->payload.data.buffer;

      tags[SAUDIO_DYNAMIC_BUFFER_ADDRESS_INDEX].ta_Tag =
        SAUDIO_AUDIO_TAG_ADDRESS;
      tags[SAUDIO_DYNAMIC_BUFFER_ADDRESS_INDEX].ta_Arg =
        (void *)&sample_chunk->samples[0];
      tags[SAUDIO_DYNAMIC_BUFFER_SIZE_INDEX].ta_Tag =
        SAUDIO_AUDIO_TAG_NUM_BYTES;
      tags[SAUDIO_DYNAMIC_BUFFER_SIZE_INDEX].ta_Arg =
        (void *)sample_chunk->sample_bytes;
      tags[SAUDIO_DYNAMIC_BUFFER_END_INDEX].ta_Tag = TAG_END;
      tags[SAUDIO_DYNAMIC_BUFFER_END_INDEX].ta_Arg = 0;
      status = SetAudioItemInfo(buffer->sample, tags);
      if(status < 0)
        {
          ReturnPoolMem(channel->buffer_pool, buffer);
          ReplyMsg(message->message_item, status, message,
                   sizeof(SAudioSubscriberMessage));
          continue;
        }

      buffer->pending_message = message;
      context->all_buffer_signals |= buffer->signal;

      if(channel->in_use_count != 0)
        {
          status = LinkAttachments(channel->in_use_queue_tail->attachment,
                                    buffer->attachment);
          if(status < 0)
            {
              buffer->pending_message = 0;
              context->all_buffer_signals &= ~buffer->signal;
              FreeBufferFromAudioFolio(buffer);
              ReturnPoolMem(channel->buffer_pool, buffer);
              ReplyMsg(message->message_item, status, message,
                       sizeof(SAudioSubscriberMessage));
              continue;
            }
        }

      AddBufferToTail(channel, buffer);
    }
}

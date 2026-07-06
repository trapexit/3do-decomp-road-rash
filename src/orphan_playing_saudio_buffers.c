#include "audio.h"

#include "saudio_subscriber_runtime.h"

void
OrphanPlayingBuffers(SAudioContext *context)
{
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  int channel_number;

  for(channel_number = 0;
      channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT;
      channel_number++)
    {
      channel = &context->channels[channel_number];
      if((channel->status & SAUDIO_CHANNEL_PLAYING_FLAGS) ==
         SAUDIO_CHANNEL_PLAYING_FLAGS)
        {
          buffer = channel->in_use_queue_head;
          if(buffer != 0)
            {
              LinkAttachments(buffer->attachment, 0);
            }
        }
    }
}

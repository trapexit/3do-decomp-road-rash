#include "audio.h"
#include "item.h"
/* Reconstructed from the original function at 0x0004BDE4. */

#include "data_stream_runtime.h"
#include "saudio_subscriber_runtime.h"

int
begin_saudio_playback_if_ready(SAudioContext *context,
                               int            channel_number)
{
  SAudioSubscriberMessage *message;
  SAudioSampleChunk *sample_chunk;
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  uint32 current_stream_time;
  uint32 current_audio_time;
  Item sleep_cue;
  int result;
  int delete_result;

  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number))
    {
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
    }
  channel = &context->channels[channel_number];

  if((channel->status & SAUDIO_CHANNEL_ENABLED_FLAG) == 0)
    {
      return channel->status;
    }
  if(channel->in_use_count == 0)
    {
      return channel->in_use_count;
    }

  buffer = channel->in_use_queue_head;
  message = buffer->pending_message;
  sample_chunk = (SAudioSampleChunk *)message->payload.data.buffer;
  if(channel->attachments_running != 0)
    {
      return channel->attachments_running;
    }

  if(channel_number == context->clock_channel)
    {
      result = DSGetClock(context->stream_control,
                          &current_stream_time);
      if(result < 0 &&
         result != ROADRASH_DS_CLOCK_NOT_VALID_ERROR)
        {
          return result;
        }
      if(result == 0 && current_stream_time < sample_chunk->stream_time)
        {
          sleep_cue = CreateSizedItem(SAUDIO_CUE_ITEM_NODE, 0, 0);
          if(sleep_cue < 0)
            {
              return sleep_cue;
            }
          current_audio_time = GetAudioTime();
          result = SleepUntilTime(
            sleep_cue,
            current_audio_time +
            (sample_chunk->stream_time - current_stream_time));
          delete_result = DeleteItem(sleep_cue);
          if(result < 0)
            {
              return result;
            }
          if(delete_result < 0)
            {
              return delete_result;
            }
        }
      result = DSSetClock(context->stream_control,
                          sample_chunk->stream_time);
      if(result < 0)
        {
          return result;
        }
    }

  result = StartAttachment(buffer->attachment, 0);
  if(result < 0)
    {
      return result;
    }
  channel->attachments_running = 1;
  return UnMuteChannel(context, channel_number,
                       SAUDIO_INTERNAL_UNMUTE);
}

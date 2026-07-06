#include "stdio.h"

#include "msgport.h"
/* Reconstructed from the original function at 0x0004B118. */

#include "data_stream_runtime.h"
#include "saudio_subscriber_runtime.h"

void
FindBuffer(SAudioContext  *context,
           uint32          signal_bits,
           SAudioChannel **channel_out,
           SAudioBuffer  **buffer_out)
{
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  unsigned int channel_signals;
  int channel_number;

  *channel_out = 0;
  *buffer_out = 0;
  channel_number = 0;
  do
    {
      channel = &context->channels[channel_number];
      channel_signals = signal_bits & channel->signal_mask;
      buffer = channel->in_use_queue_head;
      if(channel_signals != 0)
        {
          if(buffer == 0 || buffer->pending_message == 0 ||
             (channel_signals & buffer->signal) == 0)
            {
              printf("\n\n SERIOUS ERROR on channel %ld! -- A buffer "
                     "completion signal was received out of order!\n\n",
                     channel_number);
              return;
            }
          *channel_out = channel;
          *buffer_out = GetNextBuffer(channel);
          return;
        }
      channel_number++;
    } while(channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT);
}


void
HandleCompletedBuffers(SAudioContext *context,
                       uint32         signal_bits)
{
  SAudioSubscriberMessage *message;
  SAudioSampleChunk *sample_chunk;
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  unsigned int remaining_signals;
  int channel_number;
  int clock_channel_completed;
  uint32 current_stream_time;

  clock_channel_completed = 0;
  remaining_signals = signal_bits & context->all_buffer_signals;
  if(remaining_signals == 0)
    {
      return;
    }

  do
    {
      FindBuffer(context, remaining_signals, &channel, &buffer);
      if(buffer == 0)
        {
          return;
        }

      remaining_signals &= ~buffer->signal;
      context->all_buffer_signals &= ~buffer->signal;

      message = buffer->pending_message;
      sample_chunk = (SAudioSampleChunk *)message->payload.data.buffer;
      channel_number = (int)(channel - context->channels);
      if(channel_number == context->clock_channel)
        {
          clock_channel_completed = 1;
        }

      *(unsigned int *)sample_chunk->samples =
        SAUDIO_RELEASED_SAMPLE_MARKER;
      buffer->pending_message = 0;
      FreeBufferFromAudioFolio(buffer);
      ReplyMsg(message->message_item, 0, message,
               sizeof(SAudioSubscriberMessage));
      ReturnPoolMem(channel->buffer_pool, buffer);
      MoveWaitingMsgsToBufferQueue(context, channel_number);

      if(channel->in_use_count == 0 &&
         channel->pending_messages.head == 0)
        {
          channel->attachments_running = 0;
          MuteChannel(context, channel_number, SAUDIO_INTERNAL_MUTE);
        }
    } while(remaining_signals != 0);

  if(clock_channel_completed != 0 &&
     SAUDIO_CHANNEL_NUMBER_IS_VALID(context->clock_channel))
    {
      channel = &context->channels[context->clock_channel];
      buffer = channel->in_use_queue_head;
      if(buffer != 0)
        {
          message = buffer->pending_message;
          sample_chunk =
            (SAudioSampleChunk *)message->payload.data.buffer;
          if(sample_chunk->channel == context->clock_channel)
            {
              DSGetClock(context->stream_control,
                         &current_stream_time);
              DSSetClock(context->stream_control,
                         sample_chunk->stream_time);
            }
        }
    }
}

#include "audio.h"
/* CloseChannel - reconstructed from the original function at 0x0004B9D8 */

#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

long
CloseChannel(SAudioContext *context,
             long           channel_number)
{
  SAudioChannel *channel;
  SAudioOutput *output;
  SAudioBufferInitBlock buffer_context;
  long result;
  long cleanup_result;
  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number))
    {
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
    }


  channel = &context->channels[channel_number];
  output = &channel->output;

  result = FlushChannel(context, channel_number);
  if(result < 0)
    {
      return result;
    }
  cleanup_result = 0;

  if(channel->instrument > 0)
    {
      result = FreeInstrument(channel->instrument);
      if(result < 0)
        {
          cleanup_result = SAUDIO_ERROR_FREE_INSTRUMENT;
        }
      channel->instrument = 0;
    }

  if(output->instrument > 0)
    {
      StopInstrument(output->instrument, 0);
      result = FreeInstrument(output->instrument);
      if(result < 0 && cleanup_result == 0)
        {
          cleanup_result = SAUDIO_ERROR_FREE_INSTRUMENT;
        }
      output->instrument = 0;

      if(output->left_envelope > 0)
        {
          StopInstrument(output->left_envelope, 0);
          result = FreeInstrument(output->left_envelope);
          if(result < 0 && cleanup_result == 0)
            {
              cleanup_result = SAUDIO_ERROR_FREE_INSTRUMENT;
            }
          output->left_envelope = 0;
        }

      if(output->right_envelope > 0)
        {
          StopInstrument(output->right_envelope, 0);
          result = FreeInstrument(output->right_envelope);
          if(result < 0 && cleanup_result == 0)
            {
              cleanup_result = SAUDIO_ERROR_FREE_INSTRUMENT;
            }
          output->right_envelope = 0;
        }
    }

  if(channel->buffer_pool != 0)
    {
      buffer_context.channel = channel;
      ForEachFreePoolMember(channel->buffer_pool, FreeAudioBuffer,
                            &buffer_context);
      destroy_memory_pool(channel->buffer_pool);
    }

  channel->status = 0;
  channel->buffer_count = 0;
  channel->buffer_pool = 0;
  channel->instrument = 0;
  channel->instrument_started = 0;
  channel->attachments_running = 0;
  output->instrument = 0;
  output->channel_count = 0;
  output->left_envelope = 0;
  output->left_envelope_target_knob = 0;
  output->right_envelope = 0;
  output->right_envelope_target_knob = 0;
  output->current_amplitude = -1;
  output->saved_amplitude = 0;
  output->current_pan = 0;
  output->muted = 0;
  output->external_mute = 0;
  channel->signal_mask = 0;
  channel->pending_messages.head = 0;
  channel->pending_messages.tail = 0;
  channel->in_use_count = 0;
  channel->in_use_queue_head = 0;
  channel->in_use_queue_tail = 0;

  return cleanup_result;
}

#include "audio.h"
/* InitChannel - reconstructed from the original function at 0x0004B370 */

#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

static const char s_left_gain0[] = "LeftGain0";
static const char s_output[] = "Output";
static const char s_env_incr[] = "Env.incr";
static const char s_env_request[] = "Env.request";
static const char s_right_gain0[] = "RightGain0";
static const char s_right_gain1[] = "RightGain1";
static const char s_input0[] = "Input0";
static const char s_left_output[] = "LeftOutput";
static const char s_input1[] = "Input1";
static const char s_right_output[] = "RightOutput";

int
InitChannel(SAudioContext     *context,
            SAudioHeaderChunk *header)
{
  SAudioChannel *channel;
  SAudioOutput *output;
  unsigned int template_tag;
  int template_item;
  int temporary_knob;
  int status;

  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(header->channel))
    {
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
    }
  channel = &context->channels[header->channel];
  output = &channel->output;

  if(header->version > 0)
    {
      return SAUDIO_ERROR_STREAM_VERSION;
    }

  if(header->channel == 0)
    {
      channel->status |= SAUDIO_CHANNEL_ENABLED_FLAG;
    }
  channel->status |= SAUDIO_CHANNEL_ACTIVE_FLAG;

  if(output->instrument == 0)
    {
      status = AllocInstrument(context->output_template_item,
                               SAUDIO_INSTRUMENT_PRIORITY);
      if(status < 0)
        {
          return SAUDIO_ERROR_ALLOC_INSTRUMENT;
        }
      output->instrument = status;
      status = StartInstrument(output->instrument, 0);
      if(status < 0)
        {
          return status;
        }

      status = AllocInstrument(context->envelope_template_item,
                               SAUDIO_INSTRUMENT_PRIORITY);
      if(status < 0)
        {
          return SAUDIO_ERROR_ALLOC_INSTRUMENT;
        }
      output->left_envelope = status;

      status = ConnectInstruments(output->left_envelope,
                                  (char *)s_output, output->instrument,
                                  (char *)s_left_gain0);
      if(status < 0)
        {
          return SAUDIO_ERROR_CONNECT_INSTRUMENT;
        }

      temporary_knob = GrabKnob(output->left_envelope,
                                (char *)s_env_incr);
      if(temporary_knob < 0)
        {
          return SAUDIO_ERROR_GRAB_KNOB;
        }
      status = TweakRawKnob(temporary_knob, SAUDIO_ENVELOPE_RAMP_INCREMENT);
      ReleaseKnob(temporary_knob);
      if(status < 0)
        {
          return status;
        }

      temporary_knob = GrabKnob(output->left_envelope,
                                (char *)s_env_request);
      if(temporary_knob < 0)
        {
          return SAUDIO_ERROR_GRAB_KNOB;
        }
      output->left_envelope_target_knob = temporary_knob;
      status = StartInstrument(output->left_envelope, 0);
      if(status < 0)
        {
          return status;
        }

      if(header->sample.channel_count == SAUDIO_MONO_CHANNEL_COUNT)
        {
          status = AllocInstrument(context->envelope_template_item,
                                   SAUDIO_INSTRUMENT_PRIORITY);
          if(status < 0)
            {
              return SAUDIO_ERROR_ALLOC_INSTRUMENT;
            }
          output->right_envelope = status;

          status = ConnectInstruments(output->right_envelope,
                                      (char *)s_output,
                                      output->instrument,
                                      (char *)s_right_gain0);
          if(status < 0)
            {
              return SAUDIO_ERROR_CONNECT_INSTRUMENT;
            }

          temporary_knob = GrabKnob(output->right_envelope,
                                    (char *)s_env_incr);
          if(temporary_knob < 0)
            {
              return SAUDIO_ERROR_GRAB_KNOB;
            }
          status = TweakRawKnob(temporary_knob,
                                SAUDIO_ENVELOPE_RAMP_INCREMENT);
          ReleaseKnob(temporary_knob);
          if(status < 0)
            {
              return status;
            }

          temporary_knob = GrabKnob(output->right_envelope,
                                    (char *)s_env_request);
          if(temporary_knob < 0)
            {
              return SAUDIO_ERROR_GRAB_KNOB;
            }
          output->right_envelope_target_knob = temporary_knob;
          status = StartInstrument(output->right_envelope, 0);
          if(status < 0)
            {
              return status;
            }
        }
      else
        {
          status = ConnectInstruments(output->left_envelope,
                                      (char *)s_output,
                                      output->instrument,
                                      (char *)s_right_gain1);
          if(status < 0)
            {
              return SAUDIO_ERROR_CONNECT_INSTRUMENT;
            }
        }
    }

  if(channel->instrument == 0)
    {
      if(header->sample.channel_count == SAUDIO_MONO_CHANNEL_COUNT)
        {
          output->channel_count = SAUDIO_MONO_CHANNEL_COUNT;
        }
      else
        {
          output->channel_count = SAUDIO_STEREO_CHANNEL_COUNT;
        }

      channel->buffer_count = header->buffer_count;

      status = get_legacy_saudio_template_tag(&header->sample);
      if(status < 0)
        {
          return SAUDIO_ERROR_UNSUPPORTED_FORMAT;
        }
      template_tag = (unsigned int)status;

      template_item = get_legacy_saudio_template_item(
        context->template_array, template_tag, saudio_template_count);
      if(template_item < 0)
        {
          return SAUDIO_ERROR_TEMPLATE_NOT_FOUND;
        }

      status = AllocInstrument(template_item, SAUDIO_INSTRUMENT_PRIORITY);
      if(status < 0)
        {
          return SAUDIO_ERROR_ALLOC_INSTRUMENT;
        }
      channel->instrument = status;

      if(output->channel_count == SAUDIO_MONO_CHANNEL_COUNT)
        {
          status = ConnectInstruments(channel->instrument,
                                      (char *)s_output,
                                      output->instrument,
                                      (char *)s_input0);
          if(status < 0)
            {
              return SAUDIO_ERROR_CONNECT_INSTRUMENT;
            }
        }
      else
        {
          status = ConnectInstruments(channel->instrument,
                                      (char *)s_left_output,
                                      output->instrument,
                                      (char *)s_input0);
          if(status < 0)
            {
              return SAUDIO_ERROR_CONNECT_INSTRUMENT;
            }
          status = ConnectInstruments(channel->instrument,
                                      (char *)s_right_output,
                                      output->instrument,
                                      (char *)s_input1);
          if(status < 0)
            {
              return SAUDIO_ERROR_CONNECT_INSTRUMENT;
            }
        }

      status = StartInstrument(channel->instrument, 0);
      if(status < 0)
        {
          return status;
        }
      channel->instrument_started = 1;

      status = MuteChannel(context, header->channel, SAUDIO_INTERNAL_MUTE);
      if(status < 0)
        {
          return status;
        }

      if(output->saved_amplitude == -1)
        {
          status = SetChannelAmplitude(context, header->channel,
                                       header->initial_amplitude);
        }
      if(status < 0)
        {
          return status;
        }

      if(output->channel_count == SAUDIO_MONO_CHANNEL_COUNT)
        {
          if(output->current_pan == 0)
            {
              status = SetChannelPan(context, header->channel,
                                     header->initial_pan);
            }
          else
            {
              status = SetChannelPan(context, header->channel,
                                     output->current_pan);
            }
        }
      if(status < 0)
        {
          return status;
        }
    }

  if(channel->buffer_pool == 0)
    {
      status = initialize_saudio_buffer_pool(channel, header);
      if(status < 0)
        {
          return status;
        }
    }

  return 0;
}

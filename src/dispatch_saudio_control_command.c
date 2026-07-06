#include "saudio_subscriber_runtime.h"

int
dispatch_saudio_control_command(SAudioContext           *context,
                                SAudioSubscriberMessage *message)
{
  SAudioControlBlock *control;
  int channel_number;

  control = (SAudioControlBlock *)message->payload.control.data;

  switch((SAudioControlOperation)message->payload.control.argument)
    {
    case SAUDIO_CONTROL_LOAD_TEMPLATES:
      return LoadTemplates(context->template_array,
                           control->load_templates.tag_list,
                           saudio_template_count);

    case SAUDIO_CONTROL_SET_AMPLITUDE:
      channel_number = control->amplitude.channel;
      if(channel_number >= SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
        }
      return SetChannelAmplitude(
        context, channel_number, control->amplitude.value);

    case SAUDIO_CONTROL_SET_PAN:
      channel_number = control->pan.channel;
      if(channel_number >= SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
        }
      return SetChannelPan(context, channel_number, control->pan.value);

    case SAUDIO_CONTROL_SET_CLOCK_CHANNEL:
      channel_number = control->clock.channel;
      if(channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          context->clock_channel = channel_number;
          return 0;
        }
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;

    case SAUDIO_CONTROL_GET_AMPLITUDE:
      channel_number = control->amplitude.channel;
      if(channel_number >= SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
        }
      return GetChannelAmplitude(
        context, channel_number, &control->amplitude.value);

    case SAUDIO_CONTROL_GET_FREQUENCY:
      return 0;

    case SAUDIO_CONTROL_GET_PAN:
      channel_number = control->pan.channel;
      if(channel_number >= SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
        }
      return GetChannelPan(context, channel_number, &control->pan.value);

    case SAUDIO_CONTROL_GET_CLOCK_CHANNEL:
      channel_number = control->clock.channel;
      if(channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          control->clock.channel = context->clock_channel;
          return 0;
        }
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;

    case SAUDIO_CONTROL_CLOSE_CHANNEL:
      return CloseChannel(context, control->close_channel.channel);

    case SAUDIO_CONTROL_FLUSH_CHANNEL:
      return FlushChannel(context, control->flush_channel.channel);

    case SAUDIO_CONTROL_MUTE_CHANNEL:
      channel_number = control->mute.channel;
      if(channel_number >= SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
        }
      return MuteChannel(context, channel_number, SAUDIO_USER_MUTE);

    case SAUDIO_CONTROL_UNMUTE_CHANNEL:
      channel_number = control->unmute.channel;
      if(channel_number >= SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
        }
      return UnMuteChannel(context, channel_number, SAUDIO_USER_UNMUTE);

    default:
      return 0;
    }
}

#include "audio_mixer_runtime.h"

enum AudioMixerConnectionTableConstant
{
  AUDIO_MIXER_UNUSED_CONNECTION_KEY = -1,
  AUDIO_MIXER_UNUSED_SOURCE_INSTRUMENT = -1
};

void
unregister_audio_mixer_connection(AudioMixerRuntime *mixer,
                                  int                connection_key)
{
  int channel_index;
  AudioMixerChannelState *channel;

  channel_index = 0;
  if(mixer->input_count <= 0)
    {
      return;
    }

  do
    {
      channel = &mixer->channels[channel_index];
      if(channel->connection_key == connection_key)
        {
          channel->connection_key = AUDIO_MIXER_UNUSED_CONNECTION_KEY;
          channel->source_instrument =
            AUDIO_MIXER_UNUSED_SOURCE_INSTRUMENT;
          mixer->connection_count--;
          return;
        }
      channel_index++;
    } while(channel_index < mixer->input_count);
}

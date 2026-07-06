#include "audio_mixer_runtime.h"

AudioMixerChannelState *
find_audio_mixer_connection(AudioMixerRuntime *mixer,
                            int                connection_key)
{
  int channel_index;
  int input_count;
  AudioMixerChannelState *channel;

  channel_index = 0;
  input_count = mixer->input_count;
  if(input_count <= 0)
    {
      return 0;
    }

  do
    {
      channel = &mixer->channels[channel_index];
      if(channel->connection_key == connection_key)
        {
          return channel;
        }
      channel_index++;
    } while(channel_index < input_count);

  return 0;
}

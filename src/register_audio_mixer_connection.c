#include "string.h"

#include "audio_mixer_runtime.h"

enum AudioMixerConnectionTableError
{
  AUDIO_MIXER_CONNECTION_TABLE_FULL_ERROR = -28,
  AUDIO_MIXER_UNUSED_CONNECTION_KEY = -1
};

int
register_audio_mixer_connection(AudioMixerRuntime *mixer,
                                int                connection_key,
                                int                source_instrument,
                                char              *source_name)
{
  int channel_index;
  int input_count;
  AudioMixerChannelState *channel;

  channel_index = 0;
  input_count = mixer->input_count;
  if(mixer->connection_count >= input_count)
    {
      return AUDIO_MIXER_CONNECTION_TABLE_FULL_ERROR;
    }
  if(input_count <= 0)
    {
      return 0;
    }

  do
    {
      channel = &mixer->channels[channel_index];
      if(channel->connection_key ==
         AUDIO_MIXER_UNUSED_CONNECTION_KEY)
        {
          channel->connection_key = connection_key;
          channel->source_instrument = source_instrument;
          strncpy(channel->source_name, source_name,
                  sizeof(channel->source_name));
          mixer->connection_count++;
          return 0;
        }
      channel_index++;
    } while(channel_index < input_count);

  return 0;
}

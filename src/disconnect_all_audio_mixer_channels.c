#include "audio_mixer_runtime.h"

int
disconnect_all_audio_mixer_channels(AudioMixerRuntime *mixer)
{
  int channel_index;
  int result;
  int connection_item;

  result = 0;
  channel_index = 0;
  while(channel_index < mixer->input_count)
    {
      connection_item =
        mixer->channels[channel_index].connection_key;
      if(connection_item >= 0)
        {
          connection_item = disconnect_audio_mixer_source(
            mixer, connection_item);
          if(connection_item < 0)
            {
              result = connection_item;
            }
        }
      channel_index++;
    }

  return result;
}

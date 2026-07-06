#include "audio_mixer_runtime.h"

enum AudioMixerConnectionKeyConstant
{
  AUDIO_MIXER_UNAVAILABLE_CONNECTION = -1,
  AUDIO_MIXER_SUBMIXER_INDEX_SHIFT = 16
};

int
connect_audio_mixer_source(AudioMixerRuntime *mixer,
                           int                source_instrument,
                           char              *source_name)
{
  int connection_key;
  int submixer_index;
  int status;

  connection_key = AUDIO_MIXER_UNAVAILABLE_CONNECTION;
  submixer_index = 0;
  while(submixer_index < mixer->submixer_count)
    {
      status = connect_audio_submixer_input(
        mixer->submixers[submixer_index],
        source_instrument, source_name);
      if(status >= 0)
        {
          connection_key =
            status |
            (submixer_index << AUDIO_MIXER_SUBMIXER_INDEX_SHIFT);
          register_audio_mixer_connection(
            mixer, connection_key,
            source_instrument, source_name);
          break;
        }
      if(status != AUDIO_MIXER_UNAVAILABLE_CONNECTION)
        {
          connection_key = status;
          break;
        }
      submixer_index++;
    }

  if(mixer->enabled != 0 &&
     connection_key != AUDIO_MIXER_UNAVAILABLE_CONNECTION)
    {
      apply_audio_mixer_gain_levels(mixer);
    }

  return connection_key;
}

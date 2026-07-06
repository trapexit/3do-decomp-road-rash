#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

void
initialize_audio_mixer(AudioMixerRuntime *mixer,
                       int                input_count)
{
  int index;

  if(mixer == 0)
    {
      return;
    }

  index = 0;
  while(index < AUDIO_MIXER_TEMPLATE_COUNT)
    {
      mixer->template_items[index] = -1;
      mixer->submixers[index] = 0;
      index++;
    }
  mixer->submixer_count = 0;

  index = 0;
  if(input_count > 0)
    {
      do
        {
          mixer->channels[index].connection_key = -1;
          index++;
        } while(index < input_count);
    }

  mixer->connection_count = 0;
  mixer->maximum_amplitude =
    audio_mixer_template_defaults.maximum_amplitude;
  mixer->enabled = 1;
}

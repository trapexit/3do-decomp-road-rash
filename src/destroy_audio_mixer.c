#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

void
destroy_audio_mixer(AudioMixerRuntime *mixer)
{
  int index;
  int count;
  AudioSubmixerRuntime *submixer;

  if(mixer == 0)
    {
      return;
    }

  index = 0;
  disconnect_all_audio_mixer_channels(mixer);
  count = mixer->submixer_count;
  while(index < count)
    {
      submixer = mixer->submixers[index];
      if(submixer != 0)
        {
          destroy_audio_submixer(submixer);
          mixer->submixers[index] = 0;
        }
      index++;
      count = mixer->submixer_count;
    }

  mixer->submixer_count = 0;
  unload_audio_mixer_templates(mixer);

  if(mixer->channels != 0)
    {
      music_library_globals.release(
        (int)mixer->channels,
        mixer->input_count * sizeof(AudioMixerChannelState));
      mixer->channels = 0;
    }

  music_library_globals.release(
    (int)mixer, sizeof(AudioMixerRuntime));
}

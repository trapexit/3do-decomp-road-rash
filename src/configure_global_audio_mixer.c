#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

enum GlobalAudioMixerError
{
  GLOBAL_AUDIO_MIXER_CREATION_ERROR = -31
};

int
configure_global_audio_mixer(int input_count,
                             int template_image)
{
  AudioMixerRuntime *mixer;
  int result;

  result = 0;
  mixer = music_library_globals.global_mixer;
  if(mixer != 0)
    {
      if(mixer->input_count == input_count)
        {
          return result;
        }
      destroy_audio_mixer(mixer);
      music_library_globals.global_mixer = 0;
    }

  mixer = create_audio_mixer(input_count, template_image);
  music_library_globals.global_mixer = mixer;
  if(mixer == 0)
    {
      if(music_library_globals.last_error == 0)
        {
          music_library_globals.last_error =
            GLOBAL_AUDIO_MIXER_CREATION_ERROR;
        }
      result = music_library_globals.last_error;
    }
  else
    {
      set_audio_mixer_enabled(
        mixer, music_library_globals.mixer_enabled);
    }

  return result;
}

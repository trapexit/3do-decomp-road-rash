#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

int
get_global_audio_mixer_maximum_amplitude(void)
{
  return get_audio_mixer_maximum_amplitude(
    music_library_globals.global_mixer);
}

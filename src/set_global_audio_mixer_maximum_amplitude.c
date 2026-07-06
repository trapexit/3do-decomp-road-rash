#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

int
set_global_audio_mixer_maximum_amplitude(int maximum_amplitude)
{
  return set_audio_mixer_maximum_amplitude(
    music_library_globals.global_mixer, maximum_amplitude);
}

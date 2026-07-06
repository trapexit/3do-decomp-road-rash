#include "audio_mixer_runtime.h"

int
get_audio_mixer_maximum_amplitude(AudioMixerRuntime *mixer)
{
  return mixer->maximum_amplitude;
}

#include "audio_mixer_runtime.h"

int
set_audio_mixer_enabled(AudioMixerRuntime *mixer,
                        int                enabled)
{
  mixer->enabled = (unsigned char)enabled;
  return apply_audio_mixer_gain_levels(mixer);
}

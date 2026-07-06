#include "audio_mixer_runtime.h"

enum AudioMixerAmplitudeConstant
{
  AUDIO_MIXER_MAXIMUM_AMPLITUDE = 0x7FFF
};

int
set_audio_mixer_maximum_amplitude(AudioMixerRuntime *mixer,
                                  int                maximum_amplitude)
{
  if(maximum_amplitude < 0)
    {
      maximum_amplitude = 0;
    }
  else if(maximum_amplitude > AUDIO_MIXER_MAXIMUM_AMPLITUDE)
    {
      maximum_amplitude = AUDIO_MIXER_MAXIMUM_AMPLITUDE;
    }

  mixer->maximum_amplitude = maximum_amplitude;
  return apply_audio_mixer_gain_levels(mixer);
}

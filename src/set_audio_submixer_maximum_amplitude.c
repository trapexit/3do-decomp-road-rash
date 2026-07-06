#include "audio_mixer_runtime.h"

enum AudioSubmixerAmplitudeConstant
{
  AUDIO_SUBMIXER_MAXIMUM_AMPLITUDE = 0x7FFF
};

void
set_audio_submixer_maximum_amplitude(AudioSubmixerRuntime *submixer,
                                     int                   maximum_amplitude)
{
  if(maximum_amplitude < 0)
    {
      maximum_amplitude = 0;
    }
  else if(maximum_amplitude > AUDIO_SUBMIXER_MAXIMUM_AMPLITUDE)
    {
      maximum_amplitude = AUDIO_SUBMIXER_MAXIMUM_AMPLITUDE;
    }
  submixer->maximum_amplitude = maximum_amplitude;
}

#include "audio_mixer_runtime.h"
#include "division_helpers.h"

int
apply_audio_mixer_gain_levels(AudioMixerRuntime *mixer)
{
  int submixer_index;
  int maximum_amplitude;
  int submixer_count;
  AudioSubmixerRuntime *submixer;
  SignedDivisionResult division;

  submixer_index = 0;
  if(mixer->enabled != 0)
    {
      division = signed_divide_with_remainder(
        (unsigned int)mixer->connection_count,
        (unsigned int)mixer->maximum_amplitude);
      maximum_amplitude = division.quotient;
    }
  else
    {
      maximum_amplitude = mixer->maximum_amplitude;
    }

  submixer_count = mixer->submixer_count;
  if(submixer_count <= 0)
    {
      return submixer_count;
    }

  do
    {
      submixer = mixer->submixers[submixer_index];
      set_audio_submixer_maximum_amplitude(
        submixer, maximum_amplitude);
      apply_audio_submixer_input_pans(submixer);
      submixer_index++;
      submixer_count = mixer->submixer_count;
    } while(submixer_index < submixer_count);

  return submixer_count;
}

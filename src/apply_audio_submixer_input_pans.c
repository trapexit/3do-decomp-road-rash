#include "audio_mixer_runtime.h"

void
apply_audio_submixer_input_pans(AudioSubmixerRuntime *submixer)
{
  int input_index;
  int input_count;

  input_index = 0;
  input_count = submixer->input_count;
  if(input_count <= 0)
    {
      return;
    }

  do
    {
      set_audio_submixer_input_pan(
        submixer, input_index,
        submixer->left_gain_levels[input_index]);
      input_index++;
    } while(input_index < input_count);
}

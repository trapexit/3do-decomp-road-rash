#include "audio_mixer_runtime.h"

enum AudioSubmixerInitialValue
{
  AUDIO_SUBMIXER_UNASSIGNED_ITEM = -1,
  AUDIO_SUBMIXER_DEFAULT_MAXIMUM_AMPLITUDE = 0x7FFF,
  AUDIO_SUBMIXER_DEFAULT_LEFT_GAIN_LEVEL = 0x3F
};

void
initialize_audio_submixer(AudioSubmixerRuntime *submixer)
{
  int input_index;

  submixer->instrument_item = AUDIO_SUBMIXER_UNASSIGNED_ITEM;
  submixer->input_count = 0;
  submixer->maximum_amplitude =
    AUDIO_SUBMIXER_DEFAULT_MAXIMUM_AMPLITUDE;
  submixer->valid_left_amplitudes = 0;
  submixer->valid_right_amplitudes = 0;

  input_index = 0;
  while(input_index < AUDIO_SUBMIXER_INPUT_CAPACITY)
    {
      submixer->left_gain_knobs[input_index] =
        AUDIO_SUBMIXER_UNASSIGNED_ITEM;
      submixer->right_gain_knobs[input_index] =
        AUDIO_SUBMIXER_UNASSIGNED_ITEM;
      submixer->left_gain_levels[input_index] =
        AUDIO_SUBMIXER_DEFAULT_LEFT_GAIN_LEVEL;
      submixer->right_gain_levels[input_index] = 0;
      input_index++;
    }
}

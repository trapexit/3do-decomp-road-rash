#include "audio.h"
#include "stdio.h"

#include "audio_mixer_runtime.h"

enum AudioSubmixerDisconnectConstant
{
  AUDIO_SUBMIXER_INPUT_NAME_CAPACITY = 0x20
};

int
disconnect_audio_submixer_input(AudioSubmixerRuntime *submixer,
                                int                   source_instrument,
                                char                 *source_name,
                                int                   input_index)
{
  char input_name[AUDIO_SUBMIXER_INPUT_NAME_CAPACITY];
  int knob_status;
  int result;

  // The zero writes below bypass pan and can fail independently.
  submixer->valid_left_amplitudes =
    (submixer->valid_left_amplitudes & (~(1U << input_index)));
  submixer->valid_right_amplitudes =
    (submixer->valid_right_amplitudes & (~(1U << input_index)));

  sprintf(input_name, "Input%d", input_index);
  knob_status = TweakKnob(
    submixer->left_gain_knobs[input_index], 0);
  if(knob_status >= 0)
    {
      knob_status = TweakKnob(
        submixer->right_gain_knobs[input_index], 0);
    }

  result = DisconnectInstruments(
    source_instrument, source_name,
    submixer->instrument_item, input_name);
  if(knob_status < 0 || result >= 0)
    {
      submixer->right_gain_levels[input_index] = 0;
    }

  return result;
}

#include "audio.h"
#include "stdio.h"

#include "audio_mixer_runtime.h"

enum AudioSubmixerKnobConstant
{
  AUDIO_SUBMIXER_KNOB_NAME_CAPACITY = 0x100
};

int
bind_audio_submixer_input_knobs(AudioSubmixerRuntime *submixer,
                                int                   input_index)
{
  char knob_name[AUDIO_SUBMIXER_KNOB_NAME_CAPACITY];
  int knob_item;
  int result;

  result = 0;
  submixer->valid_left_amplitudes =
    (submixer->valid_left_amplitudes & (~(1U << input_index)));
  submixer->valid_right_amplitudes =
    (submixer->valid_right_amplitudes & (~(1U << input_index)));

  sprintf(knob_name, "LeftGain%ld", input_index);
  knob_item = GrabKnob(submixer->instrument_item, knob_name);
  if(knob_item >= 0)
    {
      submixer->left_gain_knobs[input_index] = knob_item;
      TweakKnob(knob_item, 0);

      sprintf(knob_name, "RightGain%ld", input_index);
      knob_item = GrabKnob(submixer->instrument_item, knob_name);
      result = knob_item;
      if(knob_item >= 0)
        {
          submixer->right_gain_knobs[input_index] = knob_item;
          result = 0;
          TweakKnob(knob_item, 0);
        }
    }
  else
    {
      result = knob_item;
    }

  return result;
}

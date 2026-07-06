#include "audio.h"

#include "audio_mixer_runtime.h"

enum AudioSubmixerPanConstant
{
  AUDIO_SUBMIXER_DEFAULT_PAN = 0x3F,
  AUDIO_SUBMIXER_MAXIMUM_PAN = 0x7F,
  AUDIO_SUBMIXER_PAN_SCALE_SHIFT = 7,
  AUDIO_SUBMIXER_SIGN_BIT_SHIFT = 31
};

int
set_audio_submixer_input_pan(AudioSubmixerRuntime *submixer,
                             int                   input_index,
                             int                   pan)
{
  int maximum_amplitude;
  int half_amplitude;
  int scaled_pan;
  int left_amplitude;
  int right_amplitude;
  int difference;
  int result;
  u32 input_bit;

  left_amplitude = 0;
  right_amplitude = 0;
  if(pan < 0)
    {
      pan = AUDIO_SUBMIXER_DEFAULT_PAN;
    }
  else if(pan > AUDIO_SUBMIXER_MAXIMUM_PAN)
    {
      pan = AUDIO_SUBMIXER_MAXIMUM_PAN;
    }

  maximum_amplitude = submixer->maximum_amplitude;
  half_amplitude =
    (maximum_amplitude +
     (int)((unsigned int)maximum_amplitude >>
           AUDIO_SUBMIXER_SIGN_BIT_SHIFT)) >> 1;
  scaled_pan =
    (pan * maximum_amplitude) >> AUDIO_SUBMIXER_PAN_SCALE_SHIFT;

  if(pan == AUDIO_SUBMIXER_DEFAULT_PAN ||
     scaled_pan == half_amplitude)
    {
      left_amplitude = maximum_amplitude;
      right_amplitude = maximum_amplitude;
    }
  else if(scaled_pan < half_amplitude)
    {
      difference = half_amplitude - scaled_pan;
      left_amplitude = maximum_amplitude;
      right_amplitude = maximum_amplitude - (difference << 1);
    }
  else if(scaled_pan > half_amplitude)
    {
      difference = scaled_pan - half_amplitude;
      left_amplitude = maximum_amplitude - (difference << 1);
      right_amplitude = maximum_amplitude;
    }

  if(left_amplitude < 0)
    {
      left_amplitude = 0;
    }
  if(right_amplitude < 0)
    {
      right_amplitude = 0;
    }

  input_bit = (1U << input_index);
  if(((submixer->valid_left_amplitudes & input_bit) == 0) ||
     (submixer->applied_left_amplitudes[input_index] != left_amplitude))
    {
      result = TweakKnob(submixer->left_gain_knobs[input_index], left_amplitude);
      if(result == 0)
        {
          submixer->applied_left_amplitudes[input_index] = left_amplitude;
          submixer->valid_left_amplitudes =
            (submixer->valid_left_amplitudes | input_bit);
        }
      else
        {
          submixer->valid_left_amplitudes =
            (submixer->valid_left_amplitudes & (~input_bit));
        }
    }

  // Preserve the original right-write status even when the left write failed.
  result = 0;
  if(((submixer->valid_right_amplitudes & input_bit) == 0) ||
     (submixer->applied_right_amplitudes[input_index] != right_amplitude))
    {
      result = TweakKnob(submixer->right_gain_knobs[input_index], right_amplitude);
      if(result == 0)
        {
          submixer->applied_right_amplitudes[input_index] = right_amplitude;
          submixer->valid_right_amplitudes =
            (submixer->valid_right_amplitudes | input_bit);
        }
      else
        {
          submixer->valid_right_amplitudes =
            (submixer->valid_right_amplitudes & (~input_bit));
        }
    }

  return result;
}

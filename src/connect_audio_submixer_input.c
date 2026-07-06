#include "audio.h"
#include "stdio.h"

#include "audio_mixer_runtime.h"

enum AudioSubmixerInputConstant
{
  AUDIO_SUBMIXER_INPUT_NAME_CAPACITY = 0x20,
  AUDIO_SUBMIXER_UNAVAILABLE_INPUT = -1,
  AUDIO_SUBMIXER_INITIAL_INPUT_PAN = 0x3F
};

int
connect_audio_submixer_input(AudioSubmixerRuntime *submixer,
                             int                   source_instrument,
                             char                 *source_name)
{
  char input_name[AUDIO_SUBMIXER_INPUT_NAME_CAPACITY];
  int result;
  int input_index;
  int input_count;

  result = AUDIO_SUBMIXER_UNAVAILABLE_INPUT;
  input_index = 0;
  input_count = submixer->input_count;
  while(input_index < input_count &&
        submixer->right_gain_levels[input_index] != 0)
    {
      input_index++;
    }

  if(input_index < input_count)
    {
      sprintf(input_name, "Input%d", input_index);
      result = ConnectInstruments(
        source_instrument, source_name,
        submixer->instrument_item, input_name);
      if(result >= 0)
        {
          result = input_index;
          submixer->right_gain_levels[input_index] = 1;
          set_audio_submixer_input_pan(
            submixer, input_index,
            AUDIO_SUBMIXER_INITIAL_INPUT_PAN);
        }
    }

  return result;
}

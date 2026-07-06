#include "audio.h"

#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

void
destroy_audio_submixer(AudioSubmixerRuntime *submixer)
{
  int input_index;
  int input_count;

  if(submixer == 0)
    {
      return;
    }

  if(submixer->instrument_item >= 0)
    {
      StopInstrument(submixer->instrument_item, 0);
    }

  input_index = 0;
  input_count = submixer->input_count;
  while(input_index < input_count)
    {
      if(submixer->left_gain_knobs[input_index] >= 0)
        {
          ReleaseKnob(submixer->left_gain_knobs[input_index]);
        }
      if(submixer->right_gain_knobs[input_index] >= 0)
        {
          ReleaseKnob(submixer->right_gain_knobs[input_index]);
        }
      input_index++;
      input_count = submixer->input_count;
    }

  if(submixer->instrument_item >= 0)
    {
      FreeInstrument(submixer->instrument_item);
    }

  music_library_globals.release(
    (int)submixer, sizeof(AudioSubmixerRuntime));
}

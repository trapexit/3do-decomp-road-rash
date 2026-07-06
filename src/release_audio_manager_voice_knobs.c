#include "audio.h"
#include "audio_manager_voice_internal.h"

void
release_audio_manager_voice_knobs(AudioManagerVoice *voice)
{
  int knob_item;

  voice->knob_cache_valid = 0;
  knob_item = voice->frequency_knob_item;
  if(knob_item >= 0)
    {
      ReleaseKnob(knob_item);
    }
  voice->frequency_knob_item = -1;

  knob_item = voice->amplitude_knob_item;
  if(knob_item < 0)
    {
      return;
    }
  ReleaseKnob(knob_item);
  voice->amplitude_knob_item = -1;
}

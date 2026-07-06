#include "audio.h"
#include "audio_manager_voice_internal.h"

int
grab_audio_manager_voice_knobs(AudioManagerVoice *voice)
{
  int frequency_knob;
  int amplitude_knob;

  voice->knob_cache_valid = 0;
  frequency_knob = GrabKnob(voice->instrument_item, "Frequency");
  amplitude_knob = GrabKnob(voice->instrument_item, "Amplitude");

  if(frequency_knob >= 0)
    {
      voice->frequency_knob_item = frequency_knob;
    }
  else
    {
      voice->frequency_knob_item = -1;
    }

  if(amplitude_knob >= 0)
    {
      voice->amplitude_knob_item = amplitude_knob;
    }
  else
    {
      voice->amplitude_knob_item = -1;
    }

  return amplitude_knob;
}

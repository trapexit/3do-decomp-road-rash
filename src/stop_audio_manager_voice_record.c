#include "audio.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

enum AudioManagerVoiceStopConstant
{
  AUDIO_MANAGER_VOICE_FADE_STEP = 0xCCC
};

int
stop_audio_manager_voice_record(volatile AudioManagerVoice *voice)
{
  int fade_value;
  int result;
  int operation_result;

  result = 0;
  // The fade and logical reset bypass the successful-tweak cache.
  voice->knob_cache_valid = 0;
  if(voice->amplitude_knob_item >= 0)
    {
      fade_value = voice->amplitude;
      do
        {
          fade_value -= AUDIO_MANAGER_VOICE_FADE_STEP;
          if(fade_value <= 0)
            {
              operation_result = TweakRawKnob(
                voice->amplitude_knob_item, 0);
            }
          else
            {
              operation_result = TweakRawKnob(
                voice->amplitude_knob_item, fade_value);
            }
          if(operation_result < 0 && result >= 0)
            {
              result = operation_result;
            }
        } while(fade_value > 0);
    }

  voice->amplitude = 0;
  voice->frequency = 0;
  operation_result = voice->instrument_item;
  if(operation_result >= 0)
    {
      operation_result = StopInstrument(operation_result, 0);
      music_library_globals.last_error = operation_result;
    }
  if(operation_result < 0 && result >= 0)
    {
      result = operation_result;
    }

  return result;
}

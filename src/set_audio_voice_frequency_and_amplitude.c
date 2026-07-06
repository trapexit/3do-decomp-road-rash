#include "audio.h"
#include "audio_manager_voice_internal.h"

enum AudioVoiceKnobConstants
{
  AUDIO_VOICE_CURRENT_AMPLITUDE_INDEX = 5,
  AUDIO_VOICE_CURRENT_FREQUENCY_INDEX = 6,
  AUDIO_VOICE_FREQUENCY_KNOB_INDEX = 8,
  AUDIO_VOICE_AMPLITUDE_KNOB_INDEX = 9,
  AUDIO_VOICE_FREQUENCY_MAX = 0xFFFF,
  AUDIO_VOICE_AMPLITUDE_MAX = 0x7FFF,
  AUDIO_VOICE_NULL_ERROR = -21
};

int
scale_audio_value_for_sample_rate(int sample_item,
                                  int value);

int
set_audio_voice_frequency_and_amplitude(int *voice,
                                        int  frequency,
                                        int  amplitude)
{
  int result;
  AudioManagerVoice *voice_state;
  AudioManagerVoiceSample *sample;

  result = 0;
  if(voice == 0)
    {
      return AUDIO_VOICE_NULL_ERROR;
    }

  voice_state = (AudioManagerVoice *)voice;

  if(voice[AUDIO_VOICE_FREQUENCY_KNOB_INDEX] >= 0 && frequency >= 0)
    {
      if(frequency > AUDIO_VOICE_FREQUENCY_MAX)
        {
          frequency = AUDIO_VOICE_FREQUENCY_MAX;
        }
      sample = get_audio_voice_sample_by_index(
        (AudioManagerVoice *)voice, 0);
      if(sample != 0)
        {
          frequency = scale_audio_value_for_sample_rate(
            sample->sample_item, frequency);
        }

      // Query and scale every request; only the effective DSP value is cached.
      if(((voice_state->knob_cache_valid &
           AUDIO_MANAGER_VOICE_FREQUENCY_CACHE_VALID) == 0) ||
         (voice[AUDIO_VOICE_CURRENT_FREQUENCY_INDEX] != frequency))
        {
          result = TweakRawKnob(
            voice[AUDIO_VOICE_FREQUENCY_KNOB_INDEX], frequency);
          if(result == 0)
            {
              voice_state->knob_cache_valid |= AUDIO_MANAGER_VOICE_FREQUENCY_CACHE_VALID;
            }
          else
            {
              voice_state->knob_cache_valid &=
                (~AUDIO_MANAGER_VOICE_FREQUENCY_CACHE_VALID);
            }
        }

      voice[AUDIO_VOICE_CURRENT_FREQUENCY_INDEX] = frequency;
    }

  if(voice[AUDIO_VOICE_AMPLITUDE_KNOB_INDEX] >= 0 && amplitude >= 0)
    {
      if(amplitude > AUDIO_VOICE_AMPLITUDE_MAX)
        {
          amplitude = AUDIO_VOICE_AMPLITUDE_MAX;
        }

      // A cached amplitude success still supersedes a preceding frequency error.
      result = 0;
      if(((voice_state->knob_cache_valid &
           AUDIO_MANAGER_VOICE_AMPLITUDE_CACHE_VALID) == 0) ||
         (voice[AUDIO_VOICE_CURRENT_AMPLITUDE_INDEX] != amplitude))
        {
          result = TweakRawKnob(
            voice[AUDIO_VOICE_AMPLITUDE_KNOB_INDEX], amplitude);
          if(result == 0)
            {
              voice_state->knob_cache_valid |= AUDIO_MANAGER_VOICE_AMPLITUDE_CACHE_VALID;
            }
          else
            {
              voice_state->knob_cache_valid &=
                (~AUDIO_MANAGER_VOICE_AMPLITUDE_CACHE_VALID);
            }
        }

      voice[AUDIO_VOICE_CURRENT_AMPLITUDE_INDEX] = amplitude;
    }

  return result;
}

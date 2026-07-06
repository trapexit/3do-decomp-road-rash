#include "audio.h"

#include "audio_manager_voice_group_internal.h"
#include "rw_semantic_data.h"

int
scale_audio_value_for_sample_rate(int sample_item,
                                  int value);

int
start_audio_manager_voice_record(AudioManagerVoice *voice,
                                 int                playback_rate,
                                 int                amplitude)
{
  TagArg *tags;
  AudioManagerVoiceSample *sample;
  int adjusted_playback_rate;
  int result;

  tags = (TagArg *)music_library_globals.start_instrument_tags;
  tags[0].ta_Tag = AF_TAG_AMPLITUDE;
  tags[1].ta_Tag = AF_TAG_RATE;

  adjusted_playback_rate = playback_rate;
  if((unsigned int)adjusted_playback_rate >
     AUDIO_MANAGER_VOICE_MAXIMUM_PLAYBACK_RATE)
    {
      adjusted_playback_rate = AUDIO_MANAGER_VOICE_MAXIMUM_PLAYBACK_RATE;
    }
  if(playback_rate < 0)
    {
      adjusted_playback_rate = AUDIO_MANAGER_VOICE_STANDARD_PLAYBACK_RATE;
    }

  if(amplitude > AUDIO_MANAGER_VOICE_MAXIMUM_AMPLITUDE)
    {
      amplitude = AUDIO_MANAGER_VOICE_MAXIMUM_AMPLITUDE;
    }
  if(amplitude < 0)
    {
      amplitude = AUDIO_MANAGER_VOICE_MAXIMUM_AMPLITUDE;
    }

  tags[0].ta_Arg = (void *)amplitude;
  voice->amplitude = amplitude;

  sample = get_audio_voice_sample_by_index(voice, 0);
  if(sample != 0)
    {
      adjusted_playback_rate = scale_audio_value_for_sample_rate(
        sample->sample_item, adjusted_playback_rate);
    }

  tags[1].ta_Arg = (void *)adjusted_playback_rate;
  voice->frequency = adjusted_playback_rate;
  tags[AUDIO_MANAGER_START_TAG_TERMINATOR_INDEX].ta_Tag = TAG_END;

  stop_audio_manager_voice_record(voice);
  if(voice->instrument_item < 0)
    {
      if(voice->group != 0)
        {
          result = allocate_or_steal_group_voice_instrument(
            voice->group, voice);
          if(result < 0)
            {
              return result;
            }
        }
      else
        {
          result = initialize_audio_manager_voice_instrument(voice);
          if(result < 0)
            {
              return result;
            }
        }
    }

  // Initialization can prime the cache, but these start tags replace both knobs.
  voice->knob_cache_valid = 0;
  return StartInstrument(voice->instrument_item, tags);
}

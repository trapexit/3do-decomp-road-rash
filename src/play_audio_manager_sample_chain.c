#include "audio_manager_voice_internal.h"

int
play_audio_manager_sample_chain(int *sample_items,
                                int  sample_count,
                                int  playback_rate,
                                int  amplitude,
                                int  pan,
                                int  requested_sort_key,
                                int  sample_chain_option)
{
  int selected_voice_index;
  AudioManagerLoadedObject *audio_object;
  AudioManagerVoice *voice;
  int result;

  selected_voice_index = 0;
  audio_object = find_audio_manager_object_for_sample(sample_items[0]);
  if(audio_object == 0)
    {
      return AUDIO_MANAGER_VOICE_UNAVAILABLE_ERROR;
    }

  voice = select_audio_manager_object_voice(
    audio_object, requested_sort_key, &selected_voice_index);
  if(voice == 0)
    {
      return AUDIO_MANAGER_NO_AUTOMATIC_VOICE_RESULT;
    }

  result = set_audio_manager_voice_record_group_sort_key(
    voice, requested_sort_key);
  if(result < 0)
    {
      return result;
    }

  result = set_audio_manager_voice_record_pan(voice, pan);
  if(result != 0)
    {
      return result;
    }

  result = replace_audio_manager_voice_samples(
    voice, sample_items, sample_count,
    AUDIO_MANAGER_STOP_INSTRUMENT_AT_SAMPLE_END);
  if(result < 0)
    {
      return result;
    }

  result = enable_audio_manager_voice_sample_linking(
    voice, sample_chain_option);
  if(result < 0)
    {
      return result;
    }

  result = start_audio_manager_voice_record(
    voice, playback_rate, amplitude);
  advance_audio_manager_object_voice_search(audio_object);
  return result;
}

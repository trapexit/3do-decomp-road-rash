#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

enum AudioManagerVoiceCreationError
{
  AUDIO_MANAGER_BAD_VOICE_TYPE_ERROR = -3
};

AudioManagerVoice *
create_audio_manager_voice_record(int template_source_kind,
                                  int instrument_template_item)
{
  int result;
  AudioManagerVoice *voice;

  result = 0;
  voice = (AudioManagerVoice *)music_library_globals.allocate(
    sizeof(AudioManagerVoice), 0);

  if(voice != 0)
    {
      voice->instrument_item = -1;
      voice->group = 0;
      voice->instrument_template_item = -1;
      voice->instrument_lock_state = 0;
      voice->is_owned = 0;
      voice->knob_cache_valid = 0;
      voice->group_sort_key = 0;
      voice->amplitude = 0;
      voice->frequency = 0;
      voice->pan = AUDIO_MANAGER_VOICE_DEFAULT_PAN;
      voice->frequency_knob_item = -1;
      voice->amplitude_knob_item = -1;
      voice->primary_output_connection_key = -1;
      voice->first_sample.next = 0;
      voice->secondary_output_connection_key = -1;
      voice->first_sample.sample_item = -1;
      voice->first_sample.attachment_item = -1;
      voice->first_sample.attachment_flags = 0;
      voice->first_sample.cue_item = -1;
      voice->sample_chain_flags = 0;

      if(template_source_kind != AUDIO_MANAGER_TEMPLATE_SOURCE_ITEM)
        {
          result = AUDIO_MANAGER_BAD_VOICE_TYPE_ERROR;
        }
      else
        {
          voice->instrument_template_item = instrument_template_item;
          if(music_library_globals.secondary_mixer_enabled == 0)
            {
              result = initialize_audio_manager_voice_instrument(voice);
            }
        }

      if(result != 0)
        {
          free_audio_manager_voice_record(voice);
          voice = 0;
          music_library_globals.last_error = result;
        }
    }

  music_library_globals.live_sound_count++;
  return voice;
}

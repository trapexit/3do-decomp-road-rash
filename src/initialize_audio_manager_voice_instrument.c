#include "audio.h"
#include "audio_manager_voice_group_internal.h"

int
set_audio_voice_frequency_and_amplitude(int *voice,
                                        int  frequency,
                                        int  amplitude);
int
initialize_audio_manager_voice_instrument(AudioManagerVoice *voice)
{
  int result;

  destroy_audio_manager_voice_instrument(voice);

  result = AllocInstrument(
    voice->instrument_template_item,
    AUDIO_MANAGER_VOICE_INSTRUMENT_PRIORITY);
  if(result < 0)
    {
      return result;
    }

  voice->instrument_item = result;
  if(voice->group != 0)
    {
      voice->group->active_voice_count++;
    }
  grab_audio_manager_voice_knobs(voice);

  result = set_audio_voice_frequency_and_amplitude(
    (int *)voice, voice->frequency, voice->amplitude);
  if(result < 0)
    {
      goto fail;
    }

  result = connect_audio_manager_voice_to_global_mixer(voice);
  if(result < 0)
    {
      goto fail;
    }

  result = set_audio_manager_voice_record_pan(voice, voice->pan);
  if(result < 0)
    {
      goto fail;
    }

  result = set_audio_manager_voice_record_group_sort_key(
    voice, voice->group_sort_key);
  if(result < 0)
    {
      goto fail;
    }

  result = attach_all_audio_manager_voice_samples(voice);
  if(result >= 0)
    {
      result = link_audio_manager_voice_sample_attachments(voice);
    }
  if(result >= 0)
    {
      return result;
    }

fail:
  destroy_audio_manager_voice_instrument(voice);
  return result;
}

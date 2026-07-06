#include "audio.h"
#include "audio_manager_voice_group_internal.h"

void
destroy_audio_manager_voice_instrument(AudioManagerVoice *voice)
{
  if(voice == 0)
    {
      return;
    }
  if(voice->instrument_item < 0)
    {
      return;
    }

  stop_audio_manager_voice_record(voice);
  detach_audio_manager_voice_sample_attachments(voice);
  disconnect_audio_manager_voice_from_global_mixer(voice);
  release_audio_manager_voice_knobs(voice);
  FreeInstrument(voice->instrument_item);

  if(voice->group != 0)
    {
      voice->group->active_voice_count--;
    }
  voice->instrument_item = -1;
  voice->instrument_lock_state = 0;
}

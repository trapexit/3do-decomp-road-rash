#include "audio_manager_voice_internal.h"

int
set_audio_manager_voice_stereo_spread(int voice_item,
                                      int spread)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }
  return set_audio_manager_voice_record_stereo_spread(voice, spread);
}

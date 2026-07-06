#include "audio_manager_voice_internal.h"

int
start_audio_manager_voice(int voice_item,
                          int playback_rate,
                          int amplitude,
                          int pan)
{
  AudioManagerVoice *voice;
  int result;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }
  if(voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  result = set_audio_manager_voice_record_pan(voice, pan);
  if(result != 0)
    {
      return result;
    }

  return start_audio_manager_voice_record(
    voice, playback_rate, amplitude);
}

#include "audio.h"

#include "audio_manager_voice_internal.h"

int
pause_audio_manager_voice(int voice_item)
{
  AudioManagerVoice *voice;
  int instrument_item;
  int result;

  result = 0;
  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      result = AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }
  else
    {
      instrument_item = voice->instrument_item;
      if(instrument_item > 0)
        {
          result = PauseInstrument(instrument_item);
        }
    }

  return result;
}

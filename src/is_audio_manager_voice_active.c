#include "audio.h"

#include "audio_manager_voice_internal.h"

int
is_audio_manager_voice_active(int voice_item)
{
  AudioManagerVoice *voice;
  int status;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice != 0 && voice->is_owned != 0)
    {
      status = get_audio_manager_voice_status(voice);
      if(status >= 0 &&
         (status == AF_STARTED || status == AF_RELEASED))
        {
          return 1;
        }
    }
  return 0;
}

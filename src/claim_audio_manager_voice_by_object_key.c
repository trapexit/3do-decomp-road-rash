#include "audio_manager_voice_internal.h"

int
claim_audio_manager_voice_by_object_key(int object_key)
{
  AudioManagerLoadedObject *audio_object;
  int voice_id;

  voice_id = -1;
  audio_object = find_audio_manager_object_by_key(object_key);
  if(audio_object == 0)
    {
      return AUDIO_MANAGER_BAD_OBJECT_ERROR;
    }

  claim_audio_manager_object_voice(audio_object, &voice_id);
  return voice_id;
}

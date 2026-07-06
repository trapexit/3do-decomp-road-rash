#include "audio_manager_voice_internal.h"

AudioManagerVoice *
resolve_audio_manager_voice(int voice_item)
{
  return resolve_audio_manager_voice_details(voice_item, 0, 0, 0);
}

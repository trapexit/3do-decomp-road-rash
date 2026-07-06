#include "audio_manager_voice_group_internal.h"

AudioManagerVoiceGroup *
create_audio_manager_voice_group(int maximum_active_voices)
{
  return allocate_audio_manager_voice_group(maximum_active_voices);
}

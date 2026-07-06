#include "audio_manager_voice_group_internal.h"

void
delete_audio_manager_voice_group(AudioManagerVoiceGroup *group)
{
  free_audio_manager_voice_group(group);
}

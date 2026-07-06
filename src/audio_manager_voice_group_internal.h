#ifndef ROADRASH_AUDIO_MANAGER_VOICE_GROUP_INTERNAL_H
#define ROADRASH_AUDIO_MANAGER_VOICE_GROUP_INTERNAL_H

#include "audio_manager_voice_group.h"
#include "audio_manager_voice_internal.h"
#include "dll_runtime.h"

struct AudioManagerVoiceGroup
{
  int maximum_active_voices;
  int active_voice_count;
  int linked_voice_count;
  DLLList *voices;
};

int
compare_audio_manager_voice_group_order(int left_voice,
                                        int right_voice);
AudioManagerVoiceGroup *
allocate_audio_manager_voice_group(int maximum_active_voices);
void
free_audio_manager_voice_group(AudioManagerVoiceGroup *group);
int
link_audio_manager_voice_to_group(AudioManagerVoiceGroup  *group,
                                  AudioManagerVoicePrefix *voice);
int
unlink_audio_manager_voice_from_group(AudioManagerVoiceGroup  *group,
                                      AudioManagerVoicePrefix *voice);
int
allocate_or_steal_group_voice_instrument(AudioManagerVoiceGroup *group,
                                         AudioManagerVoice      *requested_voice);

#endif

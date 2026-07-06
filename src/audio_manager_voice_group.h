#ifndef ROADRASH_AUDIO_MANAGER_VOICE_GROUP_H
#define ROADRASH_AUDIO_MANAGER_VOICE_GROUP_H

typedef struct AudioManagerVoiceGroup AudioManagerVoiceGroup;

AudioManagerVoiceGroup *
create_audio_manager_voice_group(int maximum_active_voices);
void
delete_audio_manager_voice_group(AudioManagerVoiceGroup *group);
int
add_audio_manager_voice_to_group(AudioManagerVoiceGroup *group,
                                 int                     voice_item);
int
remove_audio_manager_voice_from_group(AudioManagerVoiceGroup *group,
                                      int                     voice_item);

#endif

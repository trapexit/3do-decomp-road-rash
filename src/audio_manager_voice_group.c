#include "audio_manager_voice_group_internal.h"
#include "rw_semantic_data.h"

enum AudioManagerVoiceGroupConstant
{
  AUDIO_MANAGER_GROUP_DEFAULT_ACTIVE_LIMIT = 1,
  AUDIO_MANAGER_GROUP_BAD_POINTER_ERROR = -21,
  AUDIO_MANAGER_GROUP_NO_MEMORY_ERROR = -27,
  AUDIO_MANAGER_VOICE_ALREADY_GROUPED_ERROR = -37
};

DLLList *
DLL_New(RwMemoryAllocator allocator,
        RwMemoryReleaser  releaser);
int
DLL_GetErr(void);
int
DLL_SetCompare(DLLList           *list,
               DLLCompareFunction compare);
int
DLL_Delete(DLLList *list);
int
DLL_Add(DLLList *list,
        int      value);
int
DLL_Remove(DLLList *list,
           int      value);

int
link_audio_manager_voice_to_group(AudioManagerVoiceGroup  *group,
                                  AudioManagerVoicePrefix *voice)
{
  int result;

  if(group == 0 || group->voices == 0)
    {
      return AUDIO_MANAGER_GROUP_BAD_POINTER_ERROR;
    }

  if(voice->group != 0 && voice->group != group)
    {
      return AUDIO_MANAGER_VOICE_ALREADY_GROUPED_ERROR;
    }
  if(voice->group == group)
    {
      return 0;
    }

  result = DLL_Add(group->voices, (int)voice);
  if(result >= 0)
    {
      group->linked_voice_count++;
      if(voice->instrument_item >= 0)
        {
          group->active_voice_count++;
        }
      voice->group = group;
    }
  return result;
}


int
unlink_audio_manager_voice_from_group(AudioManagerVoiceGroup  *group,
                                      AudioManagerVoicePrefix *voice)
{
  int result;

  if(group == 0 || group->voices == 0)
    {
      return AUDIO_MANAGER_GROUP_BAD_POINTER_ERROR;
    }

  if(voice->group != group)
    {
      return AUDIO_MANAGER_GROUP_BAD_POINTER_ERROR;
    }
  result = DLL_Remove(group->voices, (int)voice);
  if(result >= 0)
    {
      group->linked_voice_count--;
      if(voice->instrument_item >= 0)
        {
          group->active_voice_count--;
        }
      voice->group = 0;
    }

  return result;
}


int
add_audio_manager_voice_to_group(AudioManagerVoiceGroup *group,
                                 int                     voice_item)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice != 0)
    {
      return link_audio_manager_voice_to_group(
        group, (AudioManagerVoicePrefix *)voice);
    }
  return music_library_globals.last_error;
}


int
remove_audio_manager_voice_from_group(AudioManagerVoiceGroup *group,
                                      int                     voice_item)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice != 0)
    {
      return unlink_audio_manager_voice_from_group(
        group, (AudioManagerVoicePrefix *)voice);
    }
  return music_library_globals.last_error;
}


AudioManagerVoiceGroup *
allocate_audio_manager_voice_group(int maximum_active_voices)
{
  AudioManagerVoiceGroup *group;
  int result;

  result = 0;

  group = (AudioManagerVoiceGroup *)music_library_globals.allocate(
    sizeof(AudioManagerVoiceGroup), 0);
  if(group == 0)
    {
      music_library_globals.last_error =
        AUDIO_MANAGER_GROUP_NO_MEMORY_ERROR;
      return 0;
    }

  if(maximum_active_voices <= 0)
    {
      maximum_active_voices =
        AUDIO_MANAGER_GROUP_DEFAULT_ACTIVE_LIMIT;
    }

  group->active_voice_count = 0;
  group->maximum_active_voices = maximum_active_voices;
  group->linked_voice_count = 0;
  group->voices = 0;
  group->voices = DLL_New(
    music_library_globals.allocate,
    music_library_globals.release);

  if(group->voices == 0)
    {
      result = DLL_GetErr();
      if(result >= 0)
        {
          result = -1;
        }
    }
  else
    {
      result = DLL_SetCompare(
        group->voices,
        compare_audio_manager_voice_group_order);
    }

  if(result < 0)
    {
      music_library_globals.last_error = result;
      free_audio_manager_voice_group(group);
      group = 0;
    }

  return group;
}


void
free_audio_manager_voice_group(AudioManagerVoiceGroup *group)
{
  DLLNode *node;

  if(group == 0)
    {
      return;
    }

  if(group->voices != 0)
    {
      node = group->voices->head;
      while(node != 0)
        {
          ((AudioManagerVoicePrefix *)node->value)->group = 0;
          node = node->next;
        }
      DLL_Delete(group->voices);
    }

  music_library_globals.release(
    (int)group, sizeof(AudioManagerVoiceGroup));
}

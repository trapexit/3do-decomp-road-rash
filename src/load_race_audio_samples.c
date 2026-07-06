#include "string.h"

#include "aiff_sample_runtime.h"
#include "memory_manager_runtime.h"
#include "race_audio_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RACE_AUDIO_SAMPLE_RESOURCE_FILE "Rash.AIFF"

enum RaceAudioSampleLoadConstant
{
  RACE_AUDIO_RESOURCE_PATH_CAPACITY = 0x34,
  RACE_AUDIO_FILE_LOAD_OPTIONS = 0x1000,
  RACE_AUDIO_RESOURCE_LOAD_OPTIONS = 8,
  RACE_AUDIO_SAMPLE_RESOURCE_TYPE = 0x41494646,
  RACE_AUDIO_FIRST_SAMPLE_RESOURCE_ID = 1,
  RACE_AUDIO_SAMPLE_DATA_BORROWED = 0
};

MemoryHandle *
RSRC_LoadResource(unsigned int resource_type,
                  int          resource_id,
                  int          options);

int
load_race_audio_samples(void)
{
  char resource_path[RACE_AUDIO_RESOURCE_PATH_CAPACITY];
  MemoryHandle *resource_file;
  MemoryHandle *sample_resource;
  int sample_index;
  int sample_item;

  if(game_audio_state.sound_resource_file != 0)
    {
      sample_item = unload_race_audio_samples();
      if(sample_item < 0)
        {
          return sample_item;
        }
    }
  for(sample_index = 0;
      sample_index < GAME_SOUND_RESOURCE_COUNT;
      sample_index++)
    {
      game_audio_state.sound_sample_items[sample_index] = -1;
    }

  strcpy(resource_path, resource_root_path);
  strcat(resource_path, RACE_AUDIO_SAMPLE_RESOURCE_FILE);

  resource_file =
    RSRC_OpenFile(resource_path, RACE_AUDIO_FILE_LOAD_OPTIONS);
  game_audio_state.sound_resource_file = resource_file;
  if(resource_file == 0)
    {
      sample_item = resource_system_last_error;
      return sample_item < 0 ? sample_item : -1;
    }

  for(sample_index = 0;
      sample_index < GAME_SOUND_RESOURCE_COUNT;
      sample_index++)
    {
      sample_resource = RSRC_LoadResource(
        RACE_AUDIO_SAMPLE_RESOURCE_TYPE,
        sample_index + RACE_AUDIO_FIRST_SAMPLE_RESOURCE_ID,
        RACE_AUDIO_RESOURCE_LOAD_OPTIONS);
      if(sample_resource == 0)
        {
          /* Preserve the original optional indexed-resource convention. */
          return 0;
        }

      sample_resource->flags |= MEM_HANDLE_FLAG_BUSY;
      if(sample_resource->data == 0)
        {
          sample_item = memory_system_last_error;
          return sample_item < 0 ? sample_item : -1;
        }

      /* The sample item borrows bytes held by the pinned resource. */
      sample_item = load_aiff_sample_image(
        sample_resource->data, RACE_AUDIO_SAMPLE_DATA_BORROWED);
      game_audio_state.sound_sample_items[sample_index] = sample_item;
      if(sample_item < 0)
        {
          return sample_item;
        }
    }

  return 0;
}

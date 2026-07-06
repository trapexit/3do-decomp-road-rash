#include "stdlib.h"
#include "string.h"

#include "platform.h"
#include "filefunctions.h"

#include "audio_mixer_runtime.h"
#include "audio_system.h"
#include "bss_early_data.h"
#include "rw_semantic_data.h"

char *
allocate_score_event_image(int size,
                           int flags);
int
release_score_event_image(char *memory,
                          int   size);
void
set_music_custom_memory_enabled(int enabled);
int
initialize_music_library(int *tags);

typedef struct AudioSystemTagList
{
  RwTagArgument entries[4];
  unsigned int terminator;
} AudioSystemTagList;

enum AudioSystemTagIndex
{
  AUDIO_SYSTEM_PATH_TAG_INDEX = 0,
  AUDIO_SYSTEM_ALLOCATOR_TAG_INDEX = 1,
  AUDIO_SYSTEM_RELEASER_TAG_INDEX = 2,
  AUDIO_SYSTEM_BUFFER_TAG_INDEX = 3
};

enum AudioSystemTagValue
{
  AUDIO_SYSTEM_PATH_TAG = 1,
  AUDIO_SYSTEM_ALLOCATOR_TAG = 2,
  AUDIO_SYSTEM_RELEASER_TAG = 3,
  AUDIO_SYSTEM_BUFFER_UNITS_TAG = 4,
  AUDIO_SYSTEM_BUFFER_UNITS = 8
};

int
initialize_audio_system(void)
{
  AudioSystemTagList tags;
  char *path;
  int result;

  result = ChangeDirectory("$boot");
  if(result < 0)
    {
      return result;
    }

  set_music_custom_memory_enabled(0);

  path = gAudioDirectoryPath;
  strcpy(path, (const char *)(int)&(resource_root_path));
  strcat(path, "Audio");

  tags.entries[AUDIO_SYSTEM_PATH_TAG_INDEX].tag =
    AUDIO_SYSTEM_PATH_TAG;
  tags.entries[AUDIO_SYSTEM_PATH_TAG_INDEX].argument =
    (unsigned int)path;
  tags.entries[AUDIO_SYSTEM_ALLOCATOR_TAG_INDEX].tag =
    AUDIO_SYSTEM_ALLOCATOR_TAG;
  tags.entries[AUDIO_SYSTEM_ALLOCATOR_TAG_INDEX].argument =
    (unsigned int)allocate_score_event_image;
  tags.entries[AUDIO_SYSTEM_RELEASER_TAG_INDEX].tag =
    AUDIO_SYSTEM_RELEASER_TAG;
  tags.entries[AUDIO_SYSTEM_RELEASER_TAG_INDEX].argument =
    (unsigned int)release_score_event_image;
  tags.entries[AUDIO_SYSTEM_BUFFER_TAG_INDEX].tag =
    AUDIO_SYSTEM_BUFFER_UNITS_TAG;
  tags.entries[AUDIO_SYSTEM_BUFFER_TAG_INDEX].argument =
    AUDIO_SYSTEM_BUFFER_UNITS;
  tags.terminator = 0;

  result = initialize_music_library((int *)&tags);
  set_global_audio_mixer_enabled(0);
  return result;
}

#include "filestreamfunctions.h"
#include "string.h"

#include "race_audio_runtime.h"
#include "music_file_runtime.h"
#include "rw_semantic_data.h"

int
initialize_music_context_from_tags(int tag_list);
char *
copy_audio_working_directory(char *path);
#include "memory_manager_runtime.h"

enum RaceAudioCollectionConstant
{
  RACE_AUDIO_COLLECTION_PATH_CAPACITY = 0x100,
  RACE_AUDIO_COLLECTION_TAG_COUNT = 3,
  RACE_AUDIO_COLLECTION_TERMINATOR_INDEX = 2,
  RACE_AUDIO_COLLECTION_FILE_SIZE_WORD = 9,
  RACE_AUDIO_CONTEXT_COLLECTION_IMAGE_TAG = 8,
  RACE_AUDIO_CONTEXT_SECONDARY_MIXER_TAG = 10
};

static const char race_audio_collection_path_suffix[] =
  "/AMCo/RR_SFXEnginesMusic.AMCo";

int
load_race_audio_collection(void)
{
  RwTagArgument tags[RACE_AUDIO_COLLECTION_TAG_COUNT];
  char path[RACE_AUDIO_COLLECTION_PATH_CAPACITY];
  int result;
  Stream *stream;
  char *buffer;
  int size;
  size_t path_length;
  size_t suffix_length;

  result = initialize_music_context_from_tags(0);
  if(result < 0)
    {
      return result;
    }

  copy_audio_working_directory(path);
  path_length = strlen(path);
  suffix_length = sizeof(race_audio_collection_path_suffix) - 1;
  if(path_length >= RACE_AUDIO_COLLECTION_PATH_CAPACITY ||
     suffix_length >= RACE_AUDIO_COLLECTION_PATH_CAPACITY - path_length)
    {
      return ROADRASH_MUSIC_BAD_FORMAT_ERROR;
    }
  memcpy(path + path_length, race_audio_collection_path_suffix,
         suffix_length + 1);

  stream = OpenDiskStream(path, 0);
  if(stream == 0)
    {
      return -1;
    }

  size = stream->st_FileLength;
  if(size <= 0)
    {
      result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
      goto close_stream;
    }
  buffer = MEM_NewPointer(size, MEMORY_REQUEST_ANY,
                          MEM_POINTER_OPTIONS_DEFAULT);
  if(buffer == 0)
    {
      result = ROADRASH_MUSIC_NO_MEMORY_ERROR;
      goto close_stream;
    }

  result = ReadDiskStream(stream, buffer, size);
  if(result == size)
    {
      tags[0].tag = RACE_AUDIO_CONTEXT_COLLECTION_IMAGE_TAG;
      tags[0].argument = (unsigned int)buffer;
      tags[1].tag = RACE_AUDIO_CONTEXT_SECONDARY_MIXER_TAG;
      tags[1].argument = 1;
      tags[RACE_AUDIO_COLLECTION_TERMINATOR_INDEX].tag = 0;
      result = initialize_music_context_from_tags((int)tags);
    }
  else if(result >= 0)
    {
      result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
    }
  MEM_DisposePointer(buffer);

close_stream:
  CloseDiskStream(stream);
  return result;
}

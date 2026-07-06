#include "audio.h"
#include "filefunctions.h"
#include "string.h"
/* Original ranges: 0x0003CA70-0x0003CD78. */



#include "audio_mixer_runtime.h"
#include "bss_late_data.h"
#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

#define AUDIO_DIR (gAudioWorkingDirectory)
#define AUDIO_TABLE ((int *)gLoadedAudioObjects)

#define ERR_ALLOC (-27)

#define MUSIC_DIRECTORY_CAPACITY 0x100
#define LOADED_AUDIO_OBJECT_CAPACITY 0x20
#define BLOCK_FILE_READER_BUFFER_SIZE 0x800
#define DEFAULT_AUDIO_BUFFER_UNITS 8

enum MusicLibraryInitializationTag
{
  MUSIC_LIBRARY_TAG_AUDIO_PATH = 1,
  MUSIC_LIBRARY_TAG_ALLOCATOR = 2,
  MUSIC_LIBRARY_TAG_RELEASER = 3,
  MUSIC_LIBRARY_TAG_BUFFER_UNITS = 4,
  MUSIC_LIBRARY_TAG_SUPPRESS_DIRECTORY_CHANGES = 5
};

typedef int (*AllocFn)(int size,
                             int flags);
typedef void (*FreeFn)(int ptr,
                             int size);

int
read_tag_argument_at_index(int *tag_list,
                           int  index,
                           int *out_tag,
                           int *out_value);
void
set_block_file_memory_callbacks(int allocator,
                                int releaser);
int
change_music_library_directory(char *path);
int
enter_audio_working_directory(char *saved_path,
                              int   capacity);
int
initialize_music_context_from_tags(int tag_list);
int
default_audio_manager_allocate(int size,
                               int memory_type);
void
default_audio_manager_release(int memory,
                              int size);
int
shutdown_music_library(void);

static
void
parse_init_tags(int *tags,
                      int *path,
                      int *alloc_func,
                      int *free_func,
                      int *buffer_units)
{
  int index;
  int tag;
  int value;

  index = 0;
  tag = 0;
  value = 0;

  while(tags != 0 &&
        read_tag_argument_at_index(tags, index, &tag, &value) != 0)
    {
      switch(tag)
        {
        case MUSIC_LIBRARY_TAG_AUDIO_PATH:
          *path = value;
          break;
        case MUSIC_LIBRARY_TAG_ALLOCATOR:
          *alloc_func = value;
          break;
        case MUSIC_LIBRARY_TAG_RELEASER:
          *free_func = value;
          break;
        case MUSIC_LIBRARY_TAG_BUFFER_UNITS:
          *buffer_units = value;
          break;
        case MUSIC_LIBRARY_TAG_SUPPRESS_DIRECTORY_CHANGES:
          music_library_globals.suppress_directory_changes = 1;
          break;
        default:
          break;
        }
      index++;
    }
}


static
int
select_audio_directory(char *path,
                             char *saved_dir)
{
  char *audio_dir;
  int result;
  int restore_result;

  audio_dir = AUDIO_DIR;
  if(path != 0 && path[0] != 0)
    {
      memcpy(audio_dir, ".", sizeof("."));
      result = change_music_library_directory(path);
      if(result >= 0)
        {
          result = GetDirectory(audio_dir, MUSIC_DIRECTORY_CAPACITY);
        }
    }
  else
    {
      memcpy(audio_dir, "$Audio", sizeof("$Audio"));
      result = change_music_library_directory(audio_dir);
      if(result >= 0)
        {
          result = GetDirectory(audio_dir, MUSIC_DIRECTORY_CAPACITY);
        }
    }

  if(saved_dir[0] != 0)
    {
      restore_result = change_music_library_directory(saved_dir);
      if(result >= 0 && restore_result < 0)
        {
          result = restore_result;
        }
    }
  return result;
}


static
int
setup_audio_buffer(MusicLibraryGlobals *state,
                         int                  buffer_units)
{
  int *table;
  int i;
  int result;
  int buffer;
  AllocFn alloc_func;
  FreeFn free_func;

  table = AUDIO_TABLE;
  i = 0;
  do
    {
      table[i] = 0;
      i++;
    } while(i < LOADED_AUDIO_OBJECT_CAPACITY);

  state->loaded_audio_object_count = 0;
  alloc_func = (AllocFn)state->allocate;
  buffer = alloc_func(BLOCK_FILE_READER_BUFFER_SIZE, 0);
  state->allocator_context_buffer = (void *)buffer;
  if(buffer == 0)
    {
      return ERR_ALLOC;
    }

  result = BF_InitReader((void *)buffer, state->allocate,
                         state->release);
  if(result == 1)
    {
      free_func = (FreeFn)state->release;
      free_func((int)state->allocator_context_buffer,
                BLOCK_FILE_READER_BUFFER_SIZE);
      state->allocator_context_buffer = 0;
      return configure_global_audio_mixer(buffer_units, 0);
    }
  if(result != 0)
    {
      free_func = (FreeFn)state->release;
      free_func((int)state->allocator_context_buffer,
                BLOCK_FILE_READER_BUFFER_SIZE);
      state->allocator_context_buffer = 0;
      return result;
    }

  result = configure_global_audio_mixer(buffer_units, 0);
  return result;
}


int
initialize_music_library(int *tags)
{
  MusicLibraryGlobals *state;
  char saved_dir[MUSIC_DIRECTORY_CAPACITY];
  char restore_dir[MUSIC_DIRECTORY_CAPACITY];
  int result;
  int path;
  int alloc_func;
  int free_func;
  int buffer_units;

  state = &music_library_globals;
  if(state->audio_folio_open.value != 0)
    {
      result = shutdown_music_library();
      if(result < 0)
        {
          return result;
        }
    }
  result = 0;
  path = 0;
  alloc_func = (int)default_audio_manager_allocate;
  free_func = (int)default_audio_manager_release;
  buffer_units = DEFAULT_AUDIO_BUFFER_UNITS;
  saved_dir[0] = 0;
  restore_dir[0] = 0;

  state->last_error = 0;
  state->suppress_directory_changes = 0;
  parse_init_tags(tags, &path, &alloc_func, &free_func, &buffer_units);

  state->allocate = (RwMemoryAllocator)alloc_func;
  state->release = (RwMemoryReleaser)free_func;
  set_block_file_memory_callbacks(alloc_func, free_func);

  result = GetDirectory(saved_dir, MUSIC_DIRECTORY_CAPACITY);
  if(result < 0)
    {
      return result;
    }

  result = select_audio_directory((char *)path, saved_dir);
  if(result < 0)
    {
      return result;
    }

  result = enter_audio_working_directory(
    restore_dir, MUSIC_DIRECTORY_CAPACITY);
  if(result < 0)
    {
      return result;
    }

  result = OpenAudioFolio();
  if(result == 0)
    {
      state->audio_folio_open.value = 1;
      result = setup_audio_buffer(state, buffer_units);
      if(result < 0)
        {
          shutdown_music_library();
        }
    }

  change_music_library_directory(restore_dir);
  return result;
}


int
shutdown_music_library(void)
{
  MusicLibraryGlobals *state;
  char restore_dir[MUSIC_DIRECTORY_CAPACITY];
  FreeFn free_func;

  state = &music_library_globals;
  if(state->audio_folio_open.value == 0)
    {
      return 0;
    }

  restore_dir[0] = 0;
  enter_audio_working_directory(restore_dir, MUSIC_DIRECTORY_CAPACITY);

  if(state->allocator_context_buffer != 0)
    {
      release_block_file_sector_buffer();
      free_func = (FreeFn)state->release;
      free_func((int)state->allocator_context_buffer,
                BLOCK_FILE_READER_BUFFER_SIZE);
      state->allocator_context_buffer = 0;
    }

  initialize_music_context_from_tags(0);
  destroy_audio_mixer(state->global_mixer);
  state->global_mixer = 0;
  CloseAudioFolio();
  state->audio_folio_open.value = 0;

  return change_music_library_directory(restore_dir);
}


#undef AUDIO_DIR
#undef AUDIO_TABLE
#undef ERR_ALLOC

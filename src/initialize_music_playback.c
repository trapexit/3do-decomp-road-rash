#include "platform.h"

#include "handy_tools.h"
#include "score_context_api.h"

#include "rw_semantic_data.h"

void
get_music_memory_vectors(RwMemoryAllocator *out_allocate,
                         RwMemoryReleaser  *out_release);

static RwMemoryAllocator music_allocate_vector;
static RwMemoryReleaser music_release_vector;

static
void *
allocate_music_memory(int32  size,
                      uint32 memory_type)
{
  return (void *)music_allocate_vector(size, (int)memory_type);
}


static
void
release_music_memory(void *memory,
                     int32 size)
{
  music_release_vector((int)memory, size);
}


int
initialize_music_playback(void)
{
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
  int result;

  allocate = 0;
  release = 0;
  get_music_memory_vectors(&allocate, &release);
  music_allocate_vector = allocate;
  music_release_vector = release;
  DisableScoreMessages(music_library_globals.custom_memory_enabled == 0);

  result = EZMemSetCustomVectors(allocate_music_memory,
                                 release_music_memory);
  if(result < 0)
    {
      score_loader_last_error = result;
    }

  return result;
}


void
shutdown_music_playback(void)
{
}

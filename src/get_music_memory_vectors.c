#include "rw_semantic_data.h"

void
get_music_memory_vectors(RwMemoryAllocator *out_allocate,
                         RwMemoryReleaser  *out_release)
{
  if(out_allocate != 0)
    {
      *out_allocate = music_library_globals.allocate;
    }
  if(out_release != 0)
    {
      *out_release = music_library_globals.release;
    }
}

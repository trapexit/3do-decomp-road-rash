#include "rw_semantic_data.h"

void
set_music_custom_memory_enabled(int enabled)
{
  music_library_globals.custom_memory_enabled = (unsigned char)enabled;
}

#include "audio_manager_mixer_internal.h"
#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

int
disconnect_global_mixer_connection(int connection_key)
{
  return disconnect_audio_mixer_source(
    music_library_globals.global_mixer, connection_key);
}

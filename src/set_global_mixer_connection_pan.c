#include "audio_manager_mixer_internal.h"
#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

int
set_global_mixer_connection_pan(int connection_key,
                                int pan)
{
  return set_audio_mixer_connection_pan(
    music_library_globals.global_mixer,
    connection_key, pan);
}

#include "audio_manager_mixer_internal.h"
#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

int
connect_instrument_output_to_global_mixer(int   instrument_item,
                                          char *output_name)
{
  int result;

  result = connect_audio_mixer_source(
    music_library_globals.global_mixer,
    instrument_item, output_name);
  if(result == -1)
    {
      result = AUDIO_MANAGER_MIXER_CONNECTION_ERROR;
    }
  return result;
}

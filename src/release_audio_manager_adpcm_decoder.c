#include "audio.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

int
release_audio_manager_adpcm_decoder(AudioManagerLoadedObject *audio_object)
{
  int result;
  int reference_count;

  result = 0;
  if(is_audio_manager_adpcm_instrument(
       audio_object->instrument_name) != 0)
    {
      reference_count =
        music_library_globals.decoder_reference_count - 1;
      music_library_globals.decoder_reference_count = reference_count;
      if(reference_count <= 0)
        {
          result = UnloadInstrument(
            music_library_globals.cached_decode_instrument);
          music_library_globals.cached_decode_instrument = -1;
          music_library_globals.decoder_reference_count = 0;
        }
    }
  return result;
}

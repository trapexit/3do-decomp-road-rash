#include "audio.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

enum AudioManagerAdpcmDecoderConstant
{
  AUDIO_MANAGER_ADPCM_DECODER_PRIORITY = 0xC6
};

int
retain_audio_manager_adpcm_decoder(AudioManagerLoadedObject *audio_object)
{
  int result;
  int decoder_item;

  result = 0;
  if(is_audio_manager_adpcm_instrument(
       audio_object->instrument_name) != 0)
    {
      if(music_library_globals.decoder_reference_count <= 0)
        {
          decoder_item = LoadInstrument(
            "decodeadpcm.dsp", 0,
            AUDIO_MANAGER_ADPCM_DECODER_PRIORITY);
          music_library_globals.cached_decode_instrument = decoder_item;
          if(decoder_item < 0)
            {
              return decoder_item;
            }
          music_library_globals.decoder_reference_count = 0;
        }
      music_library_globals.decoder_reference_count++;
    }
  return result;
}

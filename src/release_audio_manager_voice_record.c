#include "audio.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

int
release_audio_manager_voice_record(AudioManagerVoice *voice)
{
  int result;

  result = voice->instrument_item;
  if(result < 0)
    {
      return result;
    }
  result = ReleaseInstrument(result, 0);
  music_library_globals.last_error = result;
  return result;
}

#include "audio_mixer_runtime.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"

int
set_global_audio_mixer_enabled(int enabled)
{
  SignedDivisionResult division;
  int enabled_value;

  enabled_value = enabled & AUDIO_MIXER_ENABLE_BYTE_MASK;
  music_library_globals.mixer_enabled = (unsigned char)enabled_value;

  if(enabled_value == 0 ||
     music_library_globals.secondary_mixer_enabled == 0)
    {
      return set_audio_mixer_enabled(
        music_library_globals.global_mixer,
        music_library_globals.mixer_enabled);
    }

  set_audio_mixer_enabled(music_library_globals.global_mixer, 0);
  division = signed_divide_with_remainder(
    (unsigned int)music_library_globals.live_sound_count,
    (unsigned int)AUDIO_MIXER_FULL_SCALE_AMPLITUDE);
  return set_audio_mixer_maximum_amplitude(
    music_library_globals.global_mixer, division.quotient);
}

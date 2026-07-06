#include "audio_manager_voice_internal.h"

int
is_audio_manager_adpcm_instrument(const char *instrument_name)
{
  if(audio_manager_strings_equal_ignore_case(
       instrument_name, "adpcmmono.dsp") != 0)
    {
      return 1;
    }
  if(audio_manager_strings_equal_ignore_case(
       instrument_name, "adpcmhalfmono.dsp") != 0)
    {
      return 1;
    }
  if(audio_manager_strings_equal_ignore_case(
       instrument_name, "adpcmstereo.dsp") != 0)
    {
      return 1;
    }
  if(audio_manager_strings_equal_ignore_case(
       instrument_name, "adpcmhalfstereo.dsp") != 0)
    {
      return 1;
    }
  return 0;
}

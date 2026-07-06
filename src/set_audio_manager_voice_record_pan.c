#include "audio_manager_mixer_internal.h"
#include "audio_manager_voice_internal.h"

int
set_audio_manager_voice_record_pan(AudioManagerVoice *voice,
                                   int                pan)
{
  int result;

  result = 0;
  if(pan < 0)
    {
      pan = AUDIO_MANAGER_MIXER_CENTER_PAN;
    }
  else if(pan > AUDIO_MANAGER_MIXER_MAXIMUM_PAN)
    {
      pan = AUDIO_MANAGER_MIXER_MAXIMUM_PAN;
    }

  if(voice->primary_output_connection_key >= 0 &&
     voice->secondary_output_connection_key < 0)
    {
      result = set_global_mixer_connection_pan(
        voice->primary_output_connection_key, pan);
      if(result < 0)
        {
          return result;
        }
    }

  voice->pan = pan;
  return result;
}

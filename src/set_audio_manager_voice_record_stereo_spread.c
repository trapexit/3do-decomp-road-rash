#include "audio_manager_mixer_internal.h"
#include "audio_manager_voice_internal.h"

int
set_audio_manager_voice_record_stereo_spread(AudioManagerVoice *voice,
                                             int                spread)
{
  int half_spread;
  int result;

  result = 0;
  if(spread < 0 || spread > AUDIO_MANAGER_MIXER_MAXIMUM_PAN)
    {
      spread = AUDIO_MANAGER_MIXER_MAXIMUM_PAN;
    }

  if(voice->primary_output_connection_key < 0 ||
     voice->secondary_output_connection_key < 0)
    {
      return result;
    }

  half_spread =
    (spread + (int)((unsigned int)spread >>
                    AUDIO_MANAGER_SIGN_BIT_SHIFT)) >> 1;
  result = set_global_mixer_connection_pan(
    voice->primary_output_connection_key,
    AUDIO_MANAGER_MIXER_CENTER_PAN - half_spread);
  if(result >= 0)
    {
      return set_global_mixer_connection_pan(
        voice->secondary_output_connection_key,
        half_spread + AUDIO_MANAGER_MIXER_CENTER_PAN);
    }
  return result;
}

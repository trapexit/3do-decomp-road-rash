#include "audio_manager_mixer_internal.h"
#include "audio_manager_voice_internal.h"

int
disconnect_audio_manager_voice_from_global_mixer(AudioManagerVoice *voice)
{
  int primary_result;
  int secondary_result;

  primary_result = 0;
  secondary_result = 0;
  if(voice->primary_output_connection_key >= 0)
    {
      primary_result = disconnect_global_mixer_connection(
        voice->primary_output_connection_key);
      voice->primary_output_connection_key = -1;
    }

  if(voice->secondary_output_connection_key >= 0)
    {
      secondary_result = disconnect_global_mixer_connection(
        voice->secondary_output_connection_key);
      voice->secondary_output_connection_key = -1;
    }

  if(primary_result >= 0)
    {
      return secondary_result;
    }
  return primary_result;
}

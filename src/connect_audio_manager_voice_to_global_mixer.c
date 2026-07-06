#include "audio_manager_mixer_internal.h"
#include "audio_manager_voice_internal.h"

int
connect_audio_manager_voice_to_global_mixer(AudioManagerVoice *voice)
{
  int primary_key;
  int secondary_key;
  int result;

  primary_key = connect_instrument_output_to_global_mixer(
    voice->instrument_item, "Output");
  if(primary_key >= 0)
    {
      voice->primary_output_connection_key = primary_key;
      voice->secondary_output_connection_key = -1;
      return 0;
    }

  primary_key = connect_instrument_output_to_global_mixer(
    voice->instrument_item, "LeftOutput");
  if(primary_key < 0)
    {
      return primary_key;
    }

  secondary_key = connect_instrument_output_to_global_mixer(
    voice->instrument_item, "RightOutput");
  if(secondary_key < 0)
    {
      return secondary_key;
    }

  result = set_global_mixer_connection_pan(
    primary_key, AUDIO_MANAGER_MIXER_LEFT_INITIAL_PAN);
  if(result < 0)
    {
      return result;
    }

  result = set_global_mixer_connection_pan(
    secondary_key, AUDIO_MANAGER_MIXER_RIGHT_INITIAL_PAN);
  if(result < 0)
    {
      return result;
    }

  voice->primary_output_connection_key = primary_key;
  voice->secondary_output_connection_key = secondary_key;
  return result;
}

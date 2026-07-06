#include "audio_mixer_runtime.h"

enum AudioMixerDisconnectConstant
{
  AUDIO_MIXER_CONNECTION_INPUT_MASK = 0xFFFF,
  AUDIO_MIXER_SUBMIXER_INDEX_SHIFT = 16,
  AUDIO_MIXER_INVALID_CONNECTION_ERROR = -32
};

int
disconnect_audio_mixer_source(AudioMixerRuntime *mixer,
                              int                connection_key)
{
  int result;
  int submixer_index;
  int input_index;
  AudioSubmixerRuntime *submixer;
  AudioMixerChannelState *connection;

  result = 0;
  if(connection_key < 0)
    {
      return result;
    }

  submixer_index =
    connection_key >> AUDIO_MIXER_SUBMIXER_INDEX_SHIFT;
  input_index = connection_key & AUDIO_MIXER_CONNECTION_INPUT_MASK;
  submixer = mixer->submixers[submixer_index];
  connection = find_audio_mixer_connection(mixer, connection_key);
  if(submixer == 0 || connection == 0)
    {
      return AUDIO_MIXER_INVALID_CONNECTION_ERROR;
    }

  disconnect_audio_submixer_input(
    submixer, connection->source_instrument,
    connection->source_name, input_index);
  unregister_audio_mixer_connection(mixer, connection_key);
  if(mixer->enabled != 0)
    {
      apply_audio_mixer_gain_levels(mixer);
    }

  return result;
}

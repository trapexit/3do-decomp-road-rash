#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

enum AudioMixerConnectionPanError
{
  AUDIO_MIXER_INVALID_CONNECTION_ERROR = -32,
  AUDIO_MIXER_CONNECTION_INPUT_MASK = 0xFFFF,
  AUDIO_MIXER_SUBMIXER_INDEX_SHIFT = 16
};

int
set_audio_mixer_connection_pan(AudioMixerRuntime *mixer,
                               int                connection_key,
                               int                pan)
{
  int result;
  int submixer_index;
  int input_index;
  AudioSubmixerRuntime *submixer;

  result = 0;
  submixer_index =
    connection_key >> AUDIO_MIXER_SUBMIXER_INDEX_SHIFT;
  input_index = connection_key & AUDIO_MIXER_CONNECTION_INPUT_MASK;
  submixer = mixer->submixers[submixer_index];
  if(submixer_index < 0 ||
     submixer_index >= mixer->submixer_count ||
     submixer == 0 || input_index >= submixer->input_count)
    {
      result = AUDIO_MIXER_INVALID_CONNECTION_ERROR;
      music_library_globals.last_error = result;
    }
  else
    {
      set_audio_submixer_input_pan(submixer, input_index, pan);
    }

  return result;
}

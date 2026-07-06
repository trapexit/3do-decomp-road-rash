#include "audio_manager_voice.h"
#include "rw_semantic_data.h"

enum StreamAudioAmplitudeConstant
{
  STREAM_AUDIO_MAXIMUM_AMPLITUDE = 0x7FFF
};

int
set_stream_audio_playback_amplitude(int amplitude)
{
  if(amplitude >= 0 &&
     amplitude <= STREAM_AUDIO_MAXIMUM_AMPLITUDE)
    {
      if(audio_playback_state.active.value != 0)
        {
          if(tweak_audio_manager_voice(
               audio_playback_state.playback_voice_id,
               -1, amplitude, -1) == 0)
            {
              audio_playback_state.amplitude = amplitude;
            }
        }
      else
        {
          audio_playback_state.amplitude = amplitude;
        }
    }

  return audio_playback_state.amplitude;
}

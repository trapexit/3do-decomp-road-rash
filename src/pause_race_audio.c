/* Reconstructed from the original function at 0x0000FCAC. */



#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

int
pause_score(void);

int
pause_race_audio(void)
{
  if(game_audio_state.initialized == 0)
    {
      return 0;
    }

  stop_race_audio_voice(game_audio_state.engine_voices[0].voice_item);
  stop_race_audio_voice(game_audio_state.engine_voices[1].voice_item);
  stop_race_audio_voice(
    game_audio_state.engine_voices[
      RACE_AUDIO_SECOND_AUXILIARY_ENGINE_VOICE].voice_item);
  stop_race_audio_voice(
    game_audio_state.effect_channels[0].channel.voice_item);
  stop_race_audio_voice(
    game_audio_state.effect_channels[0].auxiliary_voice_item);
  stop_race_audio_voice(
    game_audio_state.effect_channels[1].channel.voice_item);
  stop_race_audio_voice(
    game_audio_state.effect_channels[1].auxiliary_voice_item);
  stop_race_audio_voice(game_audio_state.pursuit_voices.horn_voice_item);
  stop_race_audio_voice(game_audio_state.pursuit_voices.siren_voice_item);
  stop_race_audio_voice(
    game_audio_state.pursuit_voices.siren_blast_voice_item);

  game_audio_state.siren_active = 0;
  return pause_score();
}

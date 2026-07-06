#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

int
initialize_race_audio_voice_state(void)
{
  int voice_index;
  int channel_index;

  for(voice_index = 0;
      voice_index < RACE_AUDIO_ENGINE_VOICE_COUNT;
      voice_index++)
    {
      game_audio_state.engine_voices[voice_index].voice_item =
        RACE_AUDIO_UNASSIGNED_ITEM;
      game_audio_state.engine_voices[voice_index].sample_item =
        RACE_AUDIO_UNASSIGNED_ITEM;
    }

  for(channel_index = 0;
      channel_index < RACE_AUDIO_EFFECT_CHANNEL_COUNT;
      channel_index++)
    {
      game_audio_state.effect_channels[channel_index].voice_group =
        RACE_AUDIO_NO_VOICE_GROUP;
      game_audio_state.effect_channels[channel_index].channel.voice_item =
        RACE_AUDIO_UNASSIGNED_ITEM;
      game_audio_state.effect_channels[channel_index].channel.sample_item =
        RACE_AUDIO_UNASSIGNED_ITEM;
      game_audio_state.effect_channels[channel_index].auxiliary_voice_item =
        RACE_AUDIO_UNASSIGNED_ITEM;
    }

  game_audio_state.pursuit_voices.voice_group =
    RACE_AUDIO_NO_VOICE_GROUP;
  game_audio_state.pursuit_voices.horn_voice_item =
    RACE_AUDIO_UNASSIGNED_ITEM;
  game_audio_state.pursuit_voices.siren_voice_item =
    RACE_AUDIO_UNASSIGNED_ITEM;
  game_audio_state.pursuit_voices.siren_blast_voice_item =
    RACE_AUDIO_UNASSIGNED_ITEM;

  return RACE_AUDIO_UNASSIGNED_ITEM;
}

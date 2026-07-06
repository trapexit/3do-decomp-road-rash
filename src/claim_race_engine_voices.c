#include "operror.h"
#include "stdio.h"

#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

static
void
report_race_engine_voice_allocation_error(int result)
{
  printf("loadEngines:AM_OwnSampleVoice\n");
  PrintfSysErr(result);
}


int
claim_race_engine_voices(void)
{
  int source;
  int result;

  source = game_audio_state.sound_sample_items[0];

  result = own_audio_manager_sample_voice(
    source, RACE_AUDIO_ENGINE_SAMPLE_MODE);
  game_audio_state.engine_voices[
    RACE_AUDIO_PRIMARY_ENGINE_VOICE].voice_item = result;
  if(result < 0)
    {
      report_race_engine_voice_allocation_error(result);
      return result;
    }
  game_audio_state.engine_voices[
    RACE_AUDIO_PRIMARY_ENGINE_VOICE].sample_item = source;

  if(race_mode_state.engine_audio_enabled == 0)
    {
      return 0;
    }

  result = own_audio_manager_sample_voice(
    source, RACE_AUDIO_ENGINE_SAMPLE_MODE);
  game_audio_state.engine_voices[
    RACE_AUDIO_FIRST_AUXILIARY_ENGINE_VOICE].voice_item = result;
  if(result < 0)
    {
      report_race_engine_voice_allocation_error(result);
      return result;
    }
  game_audio_state.engine_voices[
    RACE_AUDIO_FIRST_AUXILIARY_ENGINE_VOICE].sample_item = source;

  result = own_audio_manager_sample_voice(
    source, RACE_AUDIO_ENGINE_SAMPLE_MODE);
  game_audio_state.engine_voices[
    RACE_AUDIO_SECOND_AUXILIARY_ENGINE_VOICE].voice_item = result;
  if(result < 0)
    {
      report_race_engine_voice_allocation_error(result);
      return result;
    }
  game_audio_state.engine_voices[
    RACE_AUDIO_SECOND_AUXILIARY_ENGINE_VOICE].sample_item = source;

  return 0;
}

#include "audio_manager_voice.h"
#include "operror.h"
#include "stdio.h"

#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

int
start_race_engine_voices(void)
{
  int result;

  result = 0;
  if(race_mode_state.engine_audio_enabled == 0)
    {
      return result;
    }

  if(game_audio_state.engine_voices[
       RACE_AUDIO_PRIMARY_ENGINE_VOICE].voice_item < 0 ||
     game_audio_state.engine_voices[
       RACE_AUDIO_PRIMARY_ENGINE_VOICE].sample_item < 0)
    {
      result = RACE_AUDIO_UNASSIGNED_ITEM;
      printf("startEngines: engines not properly loaded!\n");
      PrintfSysErr(result);
      return result;
    }

  result = start_audio_manager_voice(
    game_audio_state.engine_voices[
      RACE_AUDIO_PRIMARY_ENGINE_VOICE].voice_item,
    RACE_AUDIO_ENGINE_START_RATE,
    RACE_AUDIO_ENGINE_START_AMPLITUDE,
    RACE_AUDIO_CENTER_PAN_REQUEST);
  if(result < 0)
    {
      printf("startEngines:AM_StartVoice\n");
      PrintfSysErr(result);
    }
  else
    {
      result = start_audio_manager_voice(
        game_audio_state.engine_voices[
          RACE_AUDIO_FIRST_AUXILIARY_ENGINE_VOICE].voice_item,
        RACE_AUDIO_ENGINE_START_RATE,
        RACE_AUDIO_ENGINE_START_AMPLITUDE,
        RACE_AUDIO_CENTER_PAN_REQUEST);
      if(result < 0)
        {
          printf("startEngines:AM_StartVoice\n");
          PrintfSysErr(result);
        }
      else
        {
          result = start_audio_manager_voice(
            game_audio_state.engine_voices[
              RACE_AUDIO_SECOND_AUXILIARY_ENGINE_VOICE].voice_item,
            RACE_AUDIO_ENGINE_START_RATE,
            RACE_AUDIO_ENGINE_START_AMPLITUDE,
            RACE_AUDIO_CENTER_PAN_REQUEST);
          if(result < 0)
            {
              printf("startEngines:AM_StartVoice\n");
              PrintfSysErr(result);
            }
        }
    }

  return result;
}

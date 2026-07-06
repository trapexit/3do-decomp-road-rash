#include "operror.h"

#include "stdio.h"

#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum PursuitSirenPlaybackConstants
{
  PURSUIT_SIREN_PITCH = 0x7C6,
  PURSUIT_SIREN_MAIN_PHASE_LAST_TICK = 0x3C0,
  PURSUIT_SIREN_BLAST_PHASE_LAST_TICK = 0xF0
};

void
update_pursuit_siren_audio(int                      siren_voice_item,
                           const SpatialAudioEvent *event)
{
  int spatial_pitch;
  int volume;
  int pan;
  int result;
  int phase_tick;
  unsigned char siren_was_active;

  calculate_spatial_audio_parameters(
    (const RacerEntity *)race_rider_state.challenge_object,
    event, &spatial_pitch, &volume, &pan);

  siren_was_active = game_audio_state.siren_active;
  if(siren_was_active == 0)
    {
      if(volume == 0)
        {
          return;
        }

      result = start_audio_manager_voice(
        siren_voice_item, PURSUIT_SIREN_PITCH, volume, pan);
      if(result >= 0)
        {
          game_audio_state.siren_active = 1;
          game_audio_state.siren_blast_active = 0;
          game_audio_state.siren_phase_ticks = 0;
          return;
        }

      printf("soundCopSiren:AM_StartVoice(vID6b_Siren)\n");
      PrintfSysErr(result);
      return;
    }

  if(volume == 0)
    {
      if(game_audio_state.siren_blast_active != 0)
        {
          result = release_audio_manager_voice(
            game_audio_state.pursuit_voices.siren_blast_voice_item);
          if(result < 0)
            {
              printf("soundCopSiren:AM_StopVoice(vID6b_SirenBlast)\n");
              PrintfSysErr(result);
            }
          game_audio_state.siren_blast_active = 0;
        }

      result = release_audio_manager_voice(siren_voice_item);
      if(result >= 0)
        {
          game_audio_state.siren_active = 0;
          return;
        }

      printf("soundCopSiren:AM_StopVoice(vID6b_Siren)\n");
      PrintfSysErr(result);
      return;
    }

  if(siren_was_active == 0)
    {
      return;
    }

  if(game_audio_state.siren_blast_active == 0)
    {
      phase_tick = game_audio_state.siren_phase_ticks;
      game_audio_state.siren_phase_ticks = phase_tick + 1;
      if(phase_tick <= PURSUIT_SIREN_MAIN_PHASE_LAST_TICK)
        {
          result = tweak_audio_manager_voice(
            siren_voice_item, PURSUIT_SIREN_PITCH, volume, pan);
          if(result < 0)
            {
              printf("soundCopSiren:AM_TweakVoice(theVoice)\n");
              PrintfSysErr(result);
            }
          return;
        }

      result = start_audio_manager_voice(
        game_audio_state.pursuit_voices.siren_blast_voice_item,
        game_audio_state.siren_blast_pitch, volume, pan);
      if(result >= 0)
        {
          game_audio_state.siren_blast_active = 1;
          game_audio_state.siren_phase_ticks = 0;
          return;
        }

      game_audio_state.siren_phase_ticks = 0;
      printf("soundCopSiren:AM_StartVoice(vID6c_SirenBlast)\n");
      PrintfSysErr(result);
      return;
    }

  phase_tick = game_audio_state.siren_phase_ticks;
  game_audio_state.siren_phase_ticks = phase_tick + 1;
  if(phase_tick <= PURSUIT_SIREN_BLAST_PHASE_LAST_TICK)
    {
      result = tweak_audio_manager_voice(
        game_audio_state.pursuit_voices.siren_blast_voice_item,
        game_audio_state.siren_blast_pitch, volume, pan);
      if(result < 0)
        {
          printf("soundCopSiren:AM_TweakVoice(theVoice)\n");
          PrintfSysErr(result);
        }
      return;
    }

  result = start_audio_manager_voice(
    siren_voice_item, PURSUIT_SIREN_PITCH, volume, pan);
  if(result >= 0)
    {
      game_audio_state.siren_blast_active = 0;
      game_audio_state.siren_phase_ticks = 0;
      return;
    }

  game_audio_state.siren_phase_ticks = 0;
  printf("soundCopSiren:AM_StartVoice(vID6c_Siren)\n");
  PrintfSysErr(result);
}

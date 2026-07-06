#include "audio_mixer_runtime.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"
#include "stdlib.h"

typedef struct UnsignedDivideResult
{
  unsigned int quotient;
  unsigned int remainder;
} UnsignedDivideResult;

__value_in_regs UnsignedDivideResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);

enum RaceAudioModulationSlot
{
  RACE_AUDIO_PLAYER_MODULATION_SLOT = 0,
  RACE_AUDIO_FIRST_ENGINE_MODULATION_SLOT = 1,
  RACE_AUDIO_SECOND_ENGINE_MODULATION_SLOT = 2,
  RACE_AUDIO_SLIDING_MODULATION_SLOT = 3,
  RACE_AUDIO_SKID_MODULATION_SLOT = 4
};

enum RaceAudioModulatedVoiceSlot
{
  RACE_AUDIO_FIRST_ENGINE_CHANNEL = 0,
  RACE_AUDIO_SECOND_ENGINE_CHANNEL = 1,
  RACE_AUDIO_SLIDING_VOICE_CHANNEL = 2,
  RACE_AUDIO_SKID_VOICE_CHANNEL = 3
};

enum RaceAudioInitializationConstant
{
  RACE_AUDIO_BASE_ENGINE_PITCH = 0x2000,
  RACE_AUDIO_RANDOM_PITCH_SPAN = 0xE000,
  RACE_AUDIO_RANDOM_STEP_SPAN = 0x1B1,
  RACE_AUDIO_MINIMUM_PITCH_STEP = 0x50,
  RACE_AUDIO_FIRST_ENGINE_VOLUME = 0x3AAA,
  RACE_AUDIO_SECOND_ENGINE_VOLUME = 0x4AAA,
  RACE_AUDIO_SLIDING_VOICE_VOLUME = 0x3FAA,
  RACE_AUDIO_SKID_VOICE_VOLUME = 0x2FAA,
  RACE_AUDIO_FIRST_ENGINE_PAN = 0x2C,
  RACE_AUDIO_SECOND_ENGINE_PAN = 0x36,
  RACE_AUDIO_SLIDING_VOICE_PAN = 0x4A,
  RACE_AUDIO_SKID_VOICE_PAN = 0x4F,
  RACE_AUDIO_MASTER_AMPLITUDE = 0x7FFF
};

static
unsigned int
unsigned_remainder(unsigned int divisor,
                   unsigned int numerator)
{
  return unsigned_divide_with_remainder(divisor, numerator).remainder;
}


int
initialize_race_audio(void)
{
  int result;
  int value;

  if(game_audio_state.initialized != 0)
    {
      return 0;
    }
  if(game_audio_state.sound_resource_file != 0 ||
     game_audio_state.transient_sample_item > 0)
    {
      result = shutdown_race_audio();
      if(result < 0)
        {
          return result;
        }
    }

  game_audio_state.queued_event_count = 0;
  game_audio_state.queued_event_write_index = 0;
  game_audio_state.queued_event_read_index = 0;
  game_audio_state.siren_active = 0;
  game_audio_state.siren_blast_active = 0;
  game_audio_state.siren_phase_ticks = 0;

  srand((int)GrafBase->gf_VBLNumber);

  if(race_mode_state.engine_audio_enabled != 0)
    {
      game_audio_state.voice_pitch[RACE_AUDIO_PLAYER_MODULATION_SLOT] =
        RACE_AUDIO_BASE_ENGINE_PITCH;

      value = rand();
      game_audio_state.voice_pitch[
        RACE_AUDIO_FIRST_ENGINE_MODULATION_SLOT] =
        unsigned_remainder(RACE_AUDIO_RANDOM_PITCH_SPAN, value) +
        RACE_AUDIO_BASE_ENGINE_PITCH;

      value = rand();
      game_audio_state.voice_pitch[
        RACE_AUDIO_SECOND_ENGINE_MODULATION_SLOT] =
        unsigned_remainder(RACE_AUDIO_RANDOM_PITCH_SPAN, value) +
        RACE_AUDIO_BASE_ENGINE_PITCH;

      value = rand();
      game_audio_state.voice_pitch[
        RACE_AUDIO_SLIDING_MODULATION_SLOT] =
        unsigned_remainder(RACE_AUDIO_RANDOM_PITCH_SPAN, value) +
        RACE_AUDIO_BASE_ENGINE_PITCH;

      value = rand();
      game_audio_state.voice_pitch[RACE_AUDIO_SKID_MODULATION_SLOT] =
        unsigned_remainder(RACE_AUDIO_RANDOM_PITCH_SPAN, value) +
        RACE_AUDIO_BASE_ENGINE_PITCH;

      value = rand();
      game_audio_state.pitch_step[RACE_AUDIO_FIRST_ENGINE_CHANNEL] =
        unsigned_remainder(RACE_AUDIO_RANDOM_STEP_SPAN, value) +
        RACE_AUDIO_MINIMUM_PITCH_STEP;

      value = rand();
      game_audio_state.pitch_step[
        RACE_AUDIO_SECOND_ENGINE_CHANNEL] =
        unsigned_remainder(RACE_AUDIO_RANDOM_STEP_SPAN, value) +
        RACE_AUDIO_MINIMUM_PITCH_STEP;

      value = rand();
      game_audio_state.pitch_step[
        RACE_AUDIO_SLIDING_VOICE_CHANNEL] =
        unsigned_remainder(RACE_AUDIO_RANDOM_STEP_SPAN, value) +
        RACE_AUDIO_MINIMUM_PITCH_STEP;

      value = rand();
      game_audio_state.pitch_step[RACE_AUDIO_SKID_VOICE_CHANNEL] =
        unsigned_remainder(RACE_AUDIO_RANDOM_STEP_SPAN, value) +
        RACE_AUDIO_MINIMUM_PITCH_STEP;

      game_audio_state.voice_volume[
        RACE_AUDIO_FIRST_ENGINE_CHANNEL] =
        RACE_AUDIO_FIRST_ENGINE_VOLUME;
      game_audio_state.voice_volume[
        RACE_AUDIO_SECOND_ENGINE_CHANNEL] =
        RACE_AUDIO_SECOND_ENGINE_VOLUME;
      game_audio_state.voice_volume[
        RACE_AUDIO_SLIDING_VOICE_CHANNEL] =
        RACE_AUDIO_SLIDING_VOICE_VOLUME;
      game_audio_state.voice_volume[
        RACE_AUDIO_SKID_VOICE_CHANNEL] =
        RACE_AUDIO_SKID_VOICE_VOLUME;
      game_audio_state.voice_pan[RACE_AUDIO_FIRST_ENGINE_CHANNEL] =
        RACE_AUDIO_FIRST_ENGINE_PAN;
      game_audio_state.voice_pan[
        RACE_AUDIO_SECOND_ENGINE_CHANNEL] =
        RACE_AUDIO_SECOND_ENGINE_PAN;
      game_audio_state.voice_pan[
        RACE_AUDIO_SLIDING_VOICE_CHANNEL] =
        RACE_AUDIO_SLIDING_VOICE_PAN;
      game_audio_state.voice_pan[RACE_AUDIO_SKID_VOICE_CHANNEL] =
        RACE_AUDIO_SKID_VOICE_PAN;
    }
  else
    {
      game_audio_state.voice_volume[
        RACE_AUDIO_FIRST_ENGINE_CHANNEL] = 0;
      game_audio_state.voice_volume[
        RACE_AUDIO_SECOND_ENGINE_CHANNEL] = 0;
      game_audio_state.voice_volume[
        RACE_AUDIO_SLIDING_VOICE_CHANNEL] = 0;
      game_audio_state.voice_volume[
        RACE_AUDIO_SKID_VOICE_CHANNEL] = 0;
    }

  initialize_race_audio_voice_state();
  result = load_race_audio_samples();
  if(result == 0)
    {
      result = claim_race_engine_voices();
    }
  if(result == 0)
    {
      result = claim_race_patch_voices_and_groups();
    }
  if(result < 0)
    {
      shutdown_race_audio();
    }

  set_global_audio_mixer_maximum_amplitude(
    RACE_AUDIO_MASTER_AMPLITUDE);
  game_audio_state.initialized = (unsigned char)(result == 0);
  return result;
}

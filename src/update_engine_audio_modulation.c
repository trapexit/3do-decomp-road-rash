#include "division_helpers.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"
#include "stdlib.h"

/* Reconstructed from the original function at 0x0000FD48. */

enum EngineAudioModulationConstants
{
  ENGINE_AUDIO_PRIMARY_VOICE = 0,
  ENGINE_AUDIO_SECONDARY_VOICE = 1,
  ENGINE_AUDIO_TERTIARY_VOICE = 2,
  ENGINE_AUDIO_FIRST_EFFECT = 0,
  ENGINE_AUDIO_SECOND_EFFECT = 1,
  ENGINE_AUDIO_SAMPLE = 0,
  ENGINE_AUDIO_MODULATED_PITCH_OFFSET = 1,
  ENGINE_AUDIO_MODULATION_CHANNELS = 4,
  ENGINE_AUDIO_CONTROL_ACCELERATING = 0x04000000,
  ENGINE_AUDIO_PITCH_CHANGE = 0x250,
  ENGINE_AUDIO_PITCH_MINIMUM = 0x2000,
  ENGINE_AUDIO_PITCH_TURN_THRESHOLD = 0xC000,
  ENGINE_AUDIO_PITCH_MAXIMUM = 0xFFFF,
  ENGINE_AUDIO_PITCH_STEP_RANGE = 0x1B1,
  ENGINE_AUDIO_PITCH_STEP_MINIMUM = 0x50,
  ENGINE_AUDIO_VOLUME_RANGE = 0x2001,
  ENGINE_AUDIO_VOLUME_MINIMUM = 0x2AAA,
  ENGINE_AUDIO_PAN_RANGE = 0x41,
  ENGINE_AUDIO_PAN_MINIMUM = 0x20,
  ENGINE_AUDIO_PRIMARY_VOLUME = 0x5AAA,
  ENGINE_AUDIO_PRIMARY_PAN = 0x40
};

static
unsigned int
random_unsigned_remainder(unsigned int divisor)
{
  return unsigned_divide_with_remainder(divisor,
                                        (unsigned int)rand()).remainder;
}


static
int
random_signed_remainder(unsigned int divisor)
{
  return signed_divide_with_remainder(divisor,
                                      (unsigned int)rand()).remainder;
}


static
void
update_engine_modulation_channel(GameAudioState *state,
                                 int             channel_index)
{
  unsigned int value;
  int step;
  int pitch_index;

  pitch_index = channel_index + ENGINE_AUDIO_MODULATED_PITCH_OFFSET;
  step = state->pitch_step[channel_index];
  value = (unsigned int)state->voice_pitch[pitch_index] +
          (unsigned int)step;
  state->voice_pitch[pitch_index] = (int)value;

  if(value > (unsigned int)ENGINE_AUDIO_PITCH_MAXIMUM)
    {
      state->voice_pitch[pitch_index] = ENGINE_AUDIO_PITCH_MAXIMUM;
      state->pitch_step[channel_index] =
        -((int)random_unsigned_remainder(
            ENGINE_AUDIO_PITCH_STEP_RANGE) +
          ENGINE_AUDIO_PITCH_STEP_MINIMUM);
      return;
    }

  if(step < 0 &&
     value < (unsigned int)ENGINE_AUDIO_PITCH_TURN_THRESHOLD)
    {
      state->pitch_step[channel_index] =
        (int)random_unsigned_remainder(ENGINE_AUDIO_PITCH_STEP_RANGE) +
        ENGINE_AUDIO_PITCH_STEP_MINIMUM;
      state->voice_volume[channel_index] =
        random_signed_remainder(ENGINE_AUDIO_VOLUME_RANGE) +
        ENGINE_AUDIO_VOLUME_MINIMUM;
      state->voice_pan[channel_index] =
        (int)random_unsigned_remainder(ENGINE_AUDIO_PAN_RANGE) +
        ENGINE_AUDIO_PAN_MINIMUM;
    }
}


void
update_engine_audio_modulation(void)
{
  int current;
  int channel_index;
  RacerEntity *player;

  if(game_audio_state.initialized == 0)
    {
      return;
    }
  if(race_mode_state.engine_audio_enabled == 0)
    {
      return;
    }

  current = game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE];
  if((track_simulation_globals.control_bits &
      ENGINE_AUDIO_CONTROL_ACCELERATING) == 0)
    {
      current -= ENGINE_AUDIO_PITCH_CHANGE;
      game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE] = current;
      if((unsigned int)current <
         (unsigned int)ENGINE_AUDIO_PITCH_MINIMUM)
        {
          game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE] =
            ENGINE_AUDIO_PITCH_MINIMUM;
        }
    }
  else
    {
      current += ENGINE_AUDIO_PITCH_CHANGE;
      game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE] = current;
      if((unsigned int)current >
         (unsigned int)ENGINE_AUDIO_PITCH_MAXIMUM)
        {
          game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE] =
            ENGINE_AUDIO_PITCH_MAXIMUM;
        }
    }

  player = (RacerEntity *)race_rider_state.player_object;
  player->engine_pitch =
    game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE];

  for(channel_index = 0;
      channel_index < ENGINE_AUDIO_MODULATION_CHANNELS;
      channel_index++)
    {
      update_engine_modulation_channel(&game_audio_state, channel_index);
    }

  play_or_update_audio_voice_slot(
    &game_audio_state.engine_voices[ENGINE_AUDIO_PRIMARY_VOICE],
    game_audio_state.sound_sample_items[ENGINE_AUDIO_SAMPLE],
    game_audio_state.voice_pitch[ENGINE_AUDIO_PRIMARY_VOICE],
    ENGINE_AUDIO_PRIMARY_VOLUME, ENGINE_AUDIO_PRIMARY_PAN);
  play_or_update_audio_voice_slot(
    &game_audio_state.engine_voices[ENGINE_AUDIO_SECONDARY_VOICE],
    game_audio_state.sound_sample_items[ENGINE_AUDIO_SAMPLE],
    game_audio_state.voice_pitch[ENGINE_AUDIO_SECONDARY_VOICE],
    game_audio_state.voice_volume[ENGINE_AUDIO_PRIMARY_VOICE],
    game_audio_state.voice_pan[ENGINE_AUDIO_PRIMARY_VOICE]);
  play_or_update_audio_voice_slot(
    &game_audio_state.engine_voices[ENGINE_AUDIO_TERTIARY_VOICE],
    game_audio_state.sound_sample_items[ENGINE_AUDIO_SAMPLE],
    game_audio_state.voice_pitch[ENGINE_AUDIO_TERTIARY_VOICE],
    game_audio_state.voice_volume[ENGINE_AUDIO_SECONDARY_VOICE],
    game_audio_state.voice_pan[ENGINE_AUDIO_SECONDARY_VOICE]);
  play_or_update_audio_voice_slot(
    &game_audio_state.effect_channels[ENGINE_AUDIO_FIRST_EFFECT].channel,
    game_audio_state.sound_sample_items[ENGINE_AUDIO_SAMPLE],
    game_audio_state.voice_pitch[ENGINE_AUDIO_MODULATION_CHANNELS - 1],
    game_audio_state.voice_volume[ENGINE_AUDIO_TERTIARY_VOICE],
    game_audio_state.voice_pan[ENGINE_AUDIO_TERTIARY_VOICE]);
  play_or_update_audio_voice_slot(
    &game_audio_state.effect_channels[ENGINE_AUDIO_SECOND_EFFECT].channel,
    game_audio_state.sound_sample_items[ENGINE_AUDIO_SAMPLE],
    game_audio_state.voice_pitch[ENGINE_AUDIO_MODULATION_CHANNELS],
    game_audio_state.voice_volume[ENGINE_AUDIO_MODULATION_CHANNELS - 1],
    game_audio_state.voice_pan[ENGINE_AUDIO_MODULATION_CHANNELS - 1]);
}

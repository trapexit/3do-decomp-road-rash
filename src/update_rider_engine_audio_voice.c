#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum RiderEngineAudioConstant
{
  RIDER_ENGINE_GAIN_FIRST_ROUNDING = 0x1FF,
  RIDER_ENGINE_GAIN_FIRST_SHIFT = 9,
  RIDER_ENGINE_GAIN_SECOND_ROUNDING = 7,
  RIDER_ENGINE_GAIN_SECOND_SHIFT = 3,
  RIDER_ENGINE_GAIN_MULTIPLIER_SHIFT = 3,
  RIDER_ENGINE_PITCH_ROUNDING = 0xFF,
  RIDER_ENGINE_PITCH_SHIFT = 8,
  RIDER_ENGINE_PAN_CENTER = 0x40,
  RIDER_ENGINE_DEFAULT_SAMPLE_INDEX = 0,
  RIDER_ENGINE_BOOST_SAMPLE_INDEX = 17,
  RIDER_ENGINE_ALTERNATE_SAMPLE_INDEX = 18
};

static
int
divide_rider_gain_by_512_toward_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_ENGINE_GAIN_FIRST_ROUNDING;
    }
  return value >> RIDER_ENGINE_GAIN_FIRST_SHIFT;
}


static
int
divide_rider_gain_by_eight_toward_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_ENGINE_GAIN_SECOND_ROUNDING;
    }
  return value >> RIDER_ENGINE_GAIN_SECOND_SHIFT;
}


static
int
divide_rider_pitch_by_256_toward_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_ENGINE_PITCH_ROUNDING;
    }
  return value >> RIDER_ENGINE_PITCH_SHIFT;
}


int
update_rider_engine_audio_voice(const RacerEntity *rider,
                                AudioVoiceSlot    *voice)
{
  int pitch;
  int volume;
  int pan;
  int sample_item;

  volume = divide_rider_gain_by_512_toward_zero(
    rider->spatial_gain_scale_8_8 * rider->engine_gain);
  volume = divide_rider_gain_by_eight_toward_zero(
    volume + (volume << RIDER_ENGINE_GAIN_MULTIPLIER_SHIFT));

  pan = rider->spatial_pan_offset + RIDER_ENGINE_PAN_CENTER;
  pitch = divide_rider_pitch_by_256_toward_zero(
    rider->spatial_pitch_scale_8_8 * rider->engine_pitch);

  if(game_audio_state.boost_sound_active != 0)
    {
      sample_item = game_audio_state.sound_sample_items[
        RIDER_ENGINE_BOOST_SAMPLE_INDEX];
    }
  else if(game_audio_state.alternate_engine_sound_active != 0)
    {
      sample_item = game_audio_state.sound_sample_items[
        RIDER_ENGINE_ALTERNATE_SAMPLE_INDEX];
    }
  else
    {
      sample_item = game_audio_state.sound_sample_items[
        RIDER_ENGINE_DEFAULT_SAMPLE_INDEX];
    }

  return play_or_update_audio_voice_slot(
    voice, sample_item, pitch, volume, pan);
}

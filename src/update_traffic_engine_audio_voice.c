#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum TrafficEngineAudioConstant
{
  TRAFFIC_ENGINE_GAIN = 0x3FFF,
  TRAFFIC_ENGINE_PITCH = 0xD6D8,
  TRAFFIC_ENGINE_PAN_CENTER = 0x40,
  TRAFFIC_ENGINE_SAMPLE_INDEX = 1,
  TRAFFIC_ENGINE_FIXED_ROUNDING = 0xFF,
  TRAFFIC_ENGINE_FIXED_SHIFT = 8
};

static
int
divide_traffic_engine_value_by_256_toward_zero(int value)
{
  if(value < 0)
    {
      value += TRAFFIC_ENGINE_FIXED_ROUNDING;
    }
  return value >> TRAFFIC_ENGINE_FIXED_SHIFT;
}


int
update_traffic_engine_audio_voice(const RacerEntity *vehicle,
                                  AudioVoiceSlot    *voice)
{
  int pitch;
  int volume;
  int pan;

  volume = divide_traffic_engine_value_by_256_toward_zero(
    vehicle->spatial_gain_scale_8_8 * TRAFFIC_ENGINE_GAIN);
  pan = vehicle->spatial_pan_offset + TRAFFIC_ENGINE_PAN_CENTER;
  pitch = divide_traffic_engine_value_by_256_toward_zero(
    vehicle->spatial_pitch_scale_8_8 * TRAFFIC_ENGINE_PITCH);

  return play_or_update_audio_voice_slot(
    voice,
    game_audio_state.sound_sample_items[TRAFFIC_ENGINE_SAMPLE_INDEX],
    pitch, volume, pan);
}

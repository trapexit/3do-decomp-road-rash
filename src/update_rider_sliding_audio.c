#include "operror.h"
#include "stdio.h"

#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum RiderSlidingAudioConstant
{
  RIDER_SLIDING_ACTIVE_THRESHOLD = 5,
  RIDER_SLIDING_PITCH_BASE = 0xFA0,
  RIDER_SLIDING_INITIAL_GAIN = 0x3E80,
  RIDER_SLIDING_GAIN_STEP = 0x1E,
  RIDER_SLIDING_MAXIMUM_GAIN = 0x3FFF
};

static const char rider_sliding_start_error[] =
  "soundRiderSliding:AM_StartVoice(theVoice)\n";
static const char rider_sliding_stop_error[] =
  "soundRiderSliding:AM_StopVoice(theVoice)\n";
static const char rider_sliding_tweak_error[] =
  "soundRiderSliding:AM_TweakVoice(theVoice)\n";

void
update_rider_sliding_audio(int                      sliding_voice_item,
                           const SpatialAudioEvent *event)
{
  int spatial_pitch;
  int spatial_volume;
  int pan;
  int slide_velocity;
  int pitch;
  int sliding_active;
  int result;

  calculate_spatial_audio_parameters(
    0, event, &spatial_pitch, &spatial_volume, &pan);

  slide_velocity = event->forward_velocity;
  pitch = slide_velocity + RIDER_SLIDING_PITCH_BASE;
  sliding_active = slide_velocity > RIDER_SLIDING_ACTIVE_THRESHOLD;

  if(sliding_active == 0)
    {
      if(is_audio_manager_voice_active(sliding_voice_item) != 0)
        {
          result = stop_audio_manager_voice(sliding_voice_item);
          if(result < 0)
            {
              printf(rider_sliding_stop_error);
              PrintfSysErr(result);
            }
        }
      return;
    }

  if(is_audio_manager_voice_active(sliding_voice_item) == 0)
    {
      game_audio_state.sliding_voice_gain = RIDER_SLIDING_INITIAL_GAIN;
      result = start_audio_manager_voice(
        sliding_voice_item, pitch, RIDER_SLIDING_INITIAL_GAIN, pan);
      if(result < 0)
        {
          printf(rider_sliding_start_error);
          PrintfSysErr(result);
        }
      return;
    }

  if(is_audio_manager_voice_active(sliding_voice_item) == 0)
    {
      return;
    }

  game_audio_state.sliding_voice_gain += RIDER_SLIDING_GAIN_STEP;

  if(pitch < RIDER_SLIDING_PITCH_BASE)
    {
      result = stop_audio_manager_voice(sliding_voice_item);
      if(result < 0)
        {
          printf(rider_sliding_stop_error);
          PrintfSysErr(result);
        }
      return;
    }

  if(game_audio_state.sliding_voice_gain > RIDER_SLIDING_MAXIMUM_GAIN)
    {
      game_audio_state.sliding_voice_gain = RIDER_SLIDING_MAXIMUM_GAIN;
    }

  result = tweak_audio_manager_voice(
    sliding_voice_item, pitch, game_audio_state.sliding_voice_gain, pan);
  if(result < 0)
    {
      printf(rider_sliding_tweak_error);
      PrintfSysErr(result);
    }
}

#include "stdlib.h"

#include "audio_manager_voice.h"
#include "division_helpers.h"
#include "race_audio_runtime.h"

enum OffroadSkidAudioConstant
{
  OFFROAD_SKID_MAXIMUM_VOLUME = 0x7FFF,
  OFFROAD_SKID_PITCH_DIVISOR = 0x191,
  OFFROAD_SKID_PITCH_BASE = 0x0CE4
};

void
play_offroad_skid_audio(int                      skid_voice_item,
                        const SpatialAudioEvent *event)
{
  UnsignedDivisionResult random_pitch;
  int ignored_pitch;
  int volume;
  int pan;

  calculate_spatial_audio_parameters(
    0, event, &ignored_pitch, &volume, &pan);
  if(volume > OFFROAD_SKID_MAXIMUM_VOLUME)
    {
      volume = OFFROAD_SKID_MAXIMUM_VOLUME;
    }

  random_pitch = unsigned_divide_with_remainder(
    OFFROAD_SKID_PITCH_DIVISOR, (unsigned int)rand());
  start_audio_manager_voice(
    skid_voice_item,
    (int)random_pitch.remainder + OFFROAD_SKID_PITCH_BASE,
    volume, pan);
}

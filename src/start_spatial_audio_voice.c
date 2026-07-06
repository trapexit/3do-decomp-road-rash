#include "audio_manager_voice.h"
#include "race_audio_runtime.h"

int
start_spatial_audio_voice(int                      voice_item,
                          const SpatialAudioEvent *event)
{
  int pitch;
  int volume;
  int pan;

  calculate_spatial_audio_parameters(
    0, event, &pitch, &volume, &pan);
  return start_audio_manager_voice(voice_item, pitch, volume, pan);
}

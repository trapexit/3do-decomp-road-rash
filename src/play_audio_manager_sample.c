#include "audio_manager_voice.h"

int
play_audio_manager_sample(int sample_item,
                          int playback_rate,
                          int amplitude,
                          int pan,
                          int requested_sort_key)
{
  return play_audio_manager_sample_chain(
    &sample_item, 1, playback_rate, amplitude,
    pan, requested_sort_key, 0);
}

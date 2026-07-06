#include "audio_manager_voice_internal.h"

int
get_audio_manager_voice_first_sample_item(AudioManagerVoice *voice)
{
  return voice->first_sample.sample_item;
}

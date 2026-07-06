#include "audio_manager_voice_internal.h"

int
enable_audio_manager_voice_sample_linking(AudioManagerVoice *voice,
                                          int                sample_chain_option)
{
  int result;

  (void)sample_chain_option;
  result = 0;
  voice->sample_chain_flags |= AUDIO_MANAGER_VOICE_SAMPLE_LINK_FLAG;
  if(voice->instrument_item >= 0)
    {
      result = link_audio_manager_voice_sample_attachments(voice);
    }
  return result;
}

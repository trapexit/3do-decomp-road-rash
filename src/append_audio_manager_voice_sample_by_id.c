#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

int
append_audio_manager_voice_sample_by_id(int voice_id,
                                        int sample_item,
                                        int sample_mode)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_id);
  if(voice != 0)
    {
      return append_audio_manager_voice_sample(
        voice, sample_item, sample_mode & AUDIO_MANAGER_BYTE_MASK);
    }

  return music_library_globals.last_error;
}

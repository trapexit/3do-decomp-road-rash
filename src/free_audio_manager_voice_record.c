#include "audio_manager_voice_group_internal.h"
#include "rw_semantic_data.h"

int
free_audio_manager_voice_record(AudioManagerVoice *voice)
{
  int live_voice_count;

  if(voice == 0)
    {
      return 0;
    }

  clear_audio_manager_voice_samples(voice);
  destroy_audio_manager_voice_instrument(voice);
  if(voice->group != 0)
    {
      unlink_audio_manager_voice_from_group(
        voice->group, (AudioManagerVoicePrefix *)voice);
    }
  music_library_globals.release((int)voice, sizeof(AudioManagerVoice));

  live_voice_count = music_library_globals.live_sound_count - 1;
  music_library_globals.live_sound_count = live_voice_count;
  return live_voice_count;
}

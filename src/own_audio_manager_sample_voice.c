#include "audio_manager_voice_internal.h"

enum AudioManagerSampleVoiceConstant
{
  AUDIO_MANAGER_SAMPLE_MODE_MASK = 0xFF
};

int
own_audio_manager_sample_voice(int sample_item,
                               int sample_mode)
{
  int object_key;
  int selection_result;
  int voice_id;

  sample_mode &= AUDIO_MANAGER_SAMPLE_MODE_MASK;
  voice_id = -1;
  object_key =
    find_compatible_audio_manager_object_key_for_sample(sample_item);
  if(object_key != 0)
    {
      voice_id = claim_audio_manager_voice_by_object_key(object_key);
      if(voice_id >= 0)
        {
          selection_result = select_audio_manager_voice_sample(
            voice_id, sample_item, sample_mode);
          if(selection_result != 0)
            {
              disown_audio_manager_voice(voice_id);
              voice_id = selection_result;
            }
        }
    }

  return voice_id;
}

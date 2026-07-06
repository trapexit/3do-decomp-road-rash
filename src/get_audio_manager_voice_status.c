#include "audio.h"

#include "audio_manager_voice_internal.h"

#define AUDIO_MANAGER_BAD_VOICE_POINTER_ERROR (-21)
#define AUDIO_MANAGER_UNALLOCATED_VOICE_STATUS 100
#define AUDIO_MANAGER_STATUS_TAG_COUNT 2

int
get_audio_manager_voice_status(AudioManagerVoice *voice)
{
  TagArg status_tags[AUDIO_MANAGER_STATUS_TAG_COUNT];
  int instrument_item;
  int status;

  if(voice == 0)
    {
      return AUDIO_MANAGER_BAD_VOICE_POINTER_ERROR;
    }

  instrument_item = voice->instrument_item;
  if(instrument_item < 0)
    {
      return AUDIO_MANAGER_UNALLOCATED_VOICE_STATUS;
    }

  status_tags[0].ta_Tag = AF_TAG_STATUS;
  status_tags[0].ta_Arg = 0;
  status_tags[1].ta_Tag = TAG_END;

  status = GetAudioItemInfo(instrument_item, status_tags);
  if(status >= 0)
    {
      status = (int)status_tags[0].ta_Arg;
    }

  return status;
}


#undef AUDIO_MANAGER_UNALLOCATED_VOICE_STATUS
#undef AUDIO_MANAGER_BAD_VOICE_POINTER_ERROR
#undef AUDIO_MANAGER_STATUS_TAG_COUNT

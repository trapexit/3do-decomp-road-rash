#include "audio.h"
#include "musicerror.h"

#include "audio_manager_voice_group_internal.h"

enum AudioManagerVoiceStealPass
{
  AUDIO_MANAGER_STEAL_STOPPED_VOICE_PASS = 0,
  AUDIO_MANAGER_STEAL_ALLOCATED_VOICE_PASS = 1,
  AUDIO_MANAGER_STEAL_PASS_COUNT = 2
};

int
allocate_or_steal_group_voice_instrument(AudioManagerVoiceGroup *group,
                                         AudioManagerVoice      *requested_voice)
{
  int result;
  AudioManagerVoice *candidate;
  int pass;
  int voice_index;
  int status;

  result = 0;
  candidate = 0;

  if(requested_voice->instrument_item > 0)
    {
      return requested_voice->instrument_item;
    }

  if(group->active_voice_count < group->maximum_active_voices)
    {
      result = initialize_audio_manager_voice_instrument(
        requested_voice);
      if(result == ML_ERR_DUPLICATE_NAME)
        {
          result = 0;
        }
    }
  else
    {
      pass = AUDIO_MANAGER_STEAL_STOPPED_VOICE_PASS;
      while(pass < AUDIO_MANAGER_STEAL_PASS_COUNT)
        {
          voice_index = 0;
          while(voice_index < group->linked_voice_count)
            {
              result = DLL_GetIndexed(
                group->voices, voice_index, (void **)&candidate);
              if(result < 0)
                {
                  goto done;
                }

              if(pass == AUDIO_MANAGER_STEAL_STOPPED_VOICE_PASS)
                {
                  status = get_audio_manager_voice_status(candidate);
                  if(status > AF_ABANDONED &&
                     status != AF_STARTED &&
                     status != AF_RELEASED)
                    {
                      /* Keep the stopped candidate. */
                    }
                  else
                    {
                      candidate = 0;
                    }
                }
              else if(pass ==
                      AUDIO_MANAGER_STEAL_ALLOCATED_VOICE_PASS)
                {
                  if(candidate->instrument_item < 0)
                    {
                      candidate = 0;
                    }
                }

              if(candidate != 0)
                {
                  destroy_audio_manager_voice_instrument(candidate);
                  candidate = 0;
                  result = initialize_audio_manager_voice_instrument(
                    requested_voice);
                  if(result < 0)
                    {
                      if(result == ML_ERR_DUPLICATE_NAME)
                        {
                          result = 0;
                        }
                      if(result < 0)
                        {
                          goto done;
                        }
                    }
                  goto done;
                }
              voice_index++;
            }
          pass++;
        }
    }

done:
  if(result != 0)
    {
      return result;
    }
  return requested_voice->instrument_item;
}

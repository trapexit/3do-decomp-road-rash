#include "audio_manager_voice_group_internal.h"

int
compare_audio_manager_voice_group_order(int left_voice,
                                        int right_voice)
{
  AudioManagerVoicePrefix *left;
  AudioManagerVoicePrefix *right;

  left = (AudioManagerVoicePrefix *)left_voice;
  right = (AudioManagerVoicePrefix *)right_voice;

  if(left == 0)
    {
      if(right == 0)
        {
          return 0;
        }
      return -1;
    }
  if(right == 0)
    {
      return 1;
    }
  return left->group_sort_key - right->group_sort_key;
}

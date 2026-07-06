#include "audio_manager_voice.h"
#include "audio_manager_voice_group.h"
#include "audio_manager_voice_internal.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

static
int
release_race_voice(int *voice_item,
                   int sample_voice)
{
  AudioManagerVoice *voice;
  int result;

  if(*voice_item < 0)
    {
      return 0;
    }
  if(sample_voice != 0)
    {
      result = disown_audio_manager_sample_voice(*voice_item);
    }
  else
    {
      result = disown_audio_manager_voice(*voice_item);
    }
  if(result < 0)
    {
      return result;
    }
  voice = resolve_audio_manager_voice(*voice_item);
  if(voice != 0)
    {
      destroy_audio_manager_voice_instrument(voice);
    }
  *voice_item = RACE_AUDIO_UNASSIGNED_ITEM;
  return 0;
}

int
release_race_audio_voice_resources(void)
{
  AudioEffectChannel *channel;
  PursuitVoiceGroup *pursuit;
  int result;
  int release_result;
  int index;

  result = release_race_engine_voices();
  for(index = 0; index < RACE_AUDIO_EFFECT_CHANNEL_COUNT; index++)
    {
      channel = &game_audio_state.effect_channels[index];
      if(channel->voice_group != 0)
        {
          delete_audio_manager_voice_group(channel->voice_group);
          channel->voice_group = RACE_AUDIO_NO_VOICE_GROUP;
        }
      release_result = release_race_voice(&channel->channel.voice_item, 1);
      if(release_result < 0 && result >= 0)
        {
          result = release_result;
        }
      release_result = release_race_voice(&channel->auxiliary_voice_item, 0);
      if(release_result < 0 && result >= 0)
        {
          result = release_result;
        }
    }

  pursuit = &game_audio_state.pursuit_voices;
  if(pursuit->voice_group != 0)
    {
      delete_audio_manager_voice_group(pursuit->voice_group);
      pursuit->voice_group = RACE_AUDIO_NO_VOICE_GROUP;
    }
  release_result = release_race_voice(&pursuit->horn_voice_item, 0);
  if(release_result < 0 && result >= 0)
    {
      result = release_result;
    }
  release_result = release_race_voice(&pursuit->siren_voice_item, 0);
  if(release_result < 0 && result >= 0)
    {
      result = release_result;
    }
  release_result = release_race_voice(&pursuit->siren_blast_voice_item, 0);
  if(release_result < 0 && result >= 0)
    {
      result = release_result;
    }
  return result;
}

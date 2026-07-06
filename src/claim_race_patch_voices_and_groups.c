#include "operror.h"
#include "stdio.h"

#include "audio_manager_voice.h"
#include "audio_manager_voice_group.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

int
claim_race_patch_voices_and_groups(void)
{
  int result;
  AudioEffectChannel *channel;
  PursuitVoiceGroup *pursuit;
  AudioManagerVoiceGroup *group;

  result = own_audio_manager_patch_voice("Sampler.dsp");
  channel = &game_audio_state.effect_channels[
    RACE_AUDIO_SLIDING_EFFECT_CHANNEL];
  channel->channel.voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  result = own_audio_manager_patch_voice("ThingSliding.pch.ins");
  channel->auxiliary_voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  group = create_audio_manager_voice_group(
    RACE_AUDIO_MAXIMUM_ACTIVE_GROUP_VOICES);
  channel->voice_group = group;
  if(group == 0)
    {
      result = music_library_globals.last_error;
      goto done;
    }

  result = add_audio_manager_voice_to_group(
    group, channel->channel.voice_item);
  if(result < 0)
    {
      goto done;
    }
  result = add_audio_manager_voice_to_group(
    channel->voice_group, channel->auxiliary_voice_item);
  if(result < 0)
    {
      goto done;
    }

  result = own_audio_manager_patch_voice("Sampler.dsp");
  channel = &game_audio_state.effect_channels[
    RACE_AUDIO_SKID_EFFECT_CHANNEL];
  channel->channel.voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  result = own_audio_manager_patch_voice("OffRoadSkid.pch.ins");
  channel->auxiliary_voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  group = create_audio_manager_voice_group(
    RACE_AUDIO_MAXIMUM_ACTIVE_GROUP_VOICES);
  channel->voice_group = group;
  if(group == 0)
    {
      result = music_library_globals.last_error;
      goto done;
    }

  result = add_audio_manager_voice_to_group(
    group, channel->channel.voice_item);
  if(result < 0)
    {
      goto done;
    }
  result = add_audio_manager_voice_to_group(
    channel->voice_group, channel->auxiliary_voice_item);
  if(result < 0)
    {
      goto done;
    }

  result = own_audio_manager_patch_voice("CarHorn_Variable.pch.ins");
  pursuit = &game_audio_state.pursuit_voices;
  pursuit->horn_voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  result = own_audio_manager_patch_voice("RR_Siren.pch.ins");
  pursuit->siren_voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  result = own_audio_manager_patch_voice("RR_SirenBlast.pch.ins");
  pursuit->siren_blast_voice_item = result;
  if(result < 0)
    {
      goto done;
    }

  group = create_audio_manager_voice_group(
    RACE_AUDIO_MAXIMUM_ACTIVE_GROUP_VOICES);
  pursuit->voice_group = group;
  if(group == 0)
    {
      result = music_library_globals.last_error;
      goto done;
    }

  result = add_audio_manager_voice_to_group(
    group, pursuit->horn_voice_item);
  if(result < 0)
    {
      goto done;
    }

  result = add_audio_manager_voice_to_group(
    pursuit->voice_group, pursuit->siren_voice_item);
  if(result >= 0)
    {
      result = add_audio_manager_voice_to_group(
        pursuit->voice_group, pursuit->siren_blast_voice_item);
    }

done:
  if(result < 0)
    {
      printf("grabVoices\n");
      PrintfSysErr(result);
      return result;
    }
  return 0;
}

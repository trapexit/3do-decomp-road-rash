#include "audio_manager_voice.h"
#include "race_audio_runtime.h"

enum AudioVoicePlaybackLimit
{
  AUDIO_VOICE_MAXIMUM_PITCH = 0xFFFF,
  AUDIO_VOICE_MAXIMUM_VOLUME = 0x7FFF,
  AUDIO_VOICE_MINIMUM_PAN = 0,
  AUDIO_VOICE_MAXIMUM_PAN = 0x7F,
  AUDIO_VOICE_SAMPLE_MODE = 1
};

int
play_or_update_audio_voice_slot(AudioVoiceSlot *voice,
                                int             sample_item,
                                int             pitch,
                                int             volume,
                                int             pan)
{
  int voice_item;
  int result;

  if(pitch > AUDIO_VOICE_MAXIMUM_PITCH)
    {
      pitch = AUDIO_VOICE_MAXIMUM_PITCH;
    }
  if(volume > AUDIO_VOICE_MAXIMUM_VOLUME)
    {
      volume = AUDIO_VOICE_MAXIMUM_VOLUME;
    }
  if(pan < AUDIO_VOICE_MINIMUM_PAN)
    {
      pan = AUDIO_VOICE_MINIMUM_PAN;
    }
  if(pan > AUDIO_VOICE_MAXIMUM_PAN)
    {
      pan = AUDIO_VOICE_MAXIMUM_PAN;
    }

  voice_item = voice->voice_item;
  if(sample_item != voice->sample_item)
    {
      result = select_audio_manager_voice_sample(
        voice_item, sample_item, AUDIO_VOICE_SAMPLE_MODE);
      if(result != 0)
        {
          return result;
        }
      voice->sample_item = sample_item;
    }

  if(is_audio_manager_voice_active(voice_item) != 0)
    {
      return tweak_audio_manager_voice(voice_item, pitch, volume, pan);
    }
  return start_audio_manager_voice(voice_item, pitch, volume, pan);
}

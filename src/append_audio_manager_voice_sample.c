#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

int
append_audio_manager_voice_sample(AudioManagerVoice *voice,
                                  int                sample_item,
                                  int                stop_instrument_at_end)
{
  AudioManagerVoiceSample *embedded_sample;
  AudioManagerVoiceSample *sample;
  AudioManagerVoiceSample *tail;
  int result;

  result = 0;
  embedded_sample = &voice->first_sample;
  if(embedded_sample->sample_item < 0)
    {
      sample = embedded_sample;
    }
  else
    {
      sample = (AudioManagerVoiceSample *)music_library_globals.allocate(
        sizeof(AudioManagerVoiceSample), 0);
      if(sample == 0)
        {
          result = AUDIO_MANAGER_VOICE_SAMPLE_ALLOCATION_ERROR;
          goto error;
        }
    }

  sample->attachment_item = -1;
  sample->next = 0;
  sample->cue_item = -1;
  sample->attachment_flags = 0;
  sample->sample_item = sample_item;
  if((stop_instrument_at_end & AUDIO_MANAGER_BYTE_MASK) != 0)
    {
      sample->attachment_flags =
        AUDIO_MANAGER_VOICE_SAMPLE_STOP_AT_END_FLAG;
    }

  if(voice->instrument_item >= 0)
    {
      result = attach_audio_manager_voice_sample(voice, sample);
      if(result < 0)
        {
          goto error;
        }
    }

  if(sample != embedded_sample)
    {
      tail = embedded_sample;
      while(tail->next != 0)
        {
          tail = tail->next;
        }
      tail->next = sample;
    }

  return result;

error:
  if(sample == embedded_sample)
    {
      sample->next = 0;
      sample->sample_item = -1;
      sample->attachment_item = -1;
    }
  else if(sample != 0)
    {
      music_library_globals.release(
        (int)sample, sizeof(AudioManagerVoiceSample));
    }

  return result;
}

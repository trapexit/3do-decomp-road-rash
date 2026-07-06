#include "audio.h"
#include "string.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

typedef int (*AudioManagerReleaseFunction)(int memory,
                                           int size);

int
destroy_audio_manager_loaded_object(AudioManagerLoadedObject *audio_object)
{
  int index;
  int voice_count;
  AudioManagerVoice *voice;
  AudioManagerReleaseFunction release;

  if(audio_object == 0)
    {
      return 0;
    }

  index = 0;
  voice_count = audio_object->voice_count;
  if(voice_count > 0)
    {
      do
        {
          voice = audio_object->voices[index];
          if(voice != 0)
            {
              free_audio_manager_voice_record(voice);
              audio_object->voices[index] = 0;
            }
          index++;
          voice_count = audio_object->voice_count;
        } while(index < voice_count);
    }

  if(audio_object->instrument_template_item >= 0)
    {
      UnloadInsTemplate(audio_object->instrument_template_item);
      audio_object->instrument_template_item = -1;
    }

  if(audio_object->decoder_retained != 0)
    {
      release_audio_manager_adpcm_decoder(audio_object);
      audio_object->decoder_retained = 0;
    }

  release = (AudioManagerReleaseFunction)music_library_globals.release;
  if(audio_object->instrument_name != 0)
    {
      release((int)audio_object->instrument_name,
              (int)strlen(audio_object->instrument_name) + 1);
      audio_object->instrument_name = 0;
    }
  return release((int)audio_object, sizeof(AudioManagerLoadedObject));
}

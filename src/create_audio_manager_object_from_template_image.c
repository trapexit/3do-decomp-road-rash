#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

AudioManagerLoadedObject *
create_audio_manager_object_from_template_image(
  const AudioManagerInstrumentTemplateImage *template_image,
  int
  voice_count,
  int
  object_key)
{
  AudioManagerLoadedObject *audio_object;
  int index;

  if(template_image == 0)
    {
      music_library_globals.last_error = AUDIO_MANAGER_BAD_OBJECT_ERROR;
      return 0;
    }
  if(music_library_globals.loaded_audio_object_count >= AUDIO_OBJECT_SLOT_COUNT)
    {
      music_library_globals.last_error = AUDIO_MANAGER_NO_MEMORY_ERROR;
      return 0;
    }


  audio_object = create_audio_manager_loaded_object(
    AUDIO_MANAGER_TEMPLATE_SOURCE_IMAGE, template_image,
    template_image->instrument_name, object_key, voice_count);
  if(audio_object != 0)
    {
      index = music_library_globals.loaded_audio_object_count;
      music_library_globals.loaded_audio_object_count = index + 1;
      gLoadedAudioObjects[index] = audio_object;
    }
  return audio_object;
}

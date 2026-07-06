#include "string.h"

#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "path_runtime.h"
#include "rw_semantic_data.h"

enum AudioManagerInstrumentNameConstant
{
  AUDIO_MANAGER_INSTRUMENT_NAME_CAPACITY = 64
};

AudioManagerLoadedObject *
load_audio_manager_object_from_file(const char *instrument_path,
                                    int         voice_count,
                                    int         object_key)
{
  char instrument_name[AUDIO_MANAGER_INSTRUMENT_NAME_CAPACITY];
  AudioManagerLoadedObject *audio_object;
  int index;

  if(music_library_globals.loaded_audio_object_count >= AUDIO_OBJECT_SLOT_COUNT)
    {
      music_library_globals.last_error = AUDIO_MANAGER_NO_MEMORY_ERROR;
      return 0;
    }

  if(copy_path_with_capacity(
       instrument_name, sizeof(instrument_name), instrument_path) < 0)
    {
      music_library_globals.last_error = PATH_RUNTIME_ERROR_TOO_LONG;
      return 0;
    }
  strip_audio_manager_instrument_path(instrument_name);
  audio_object = create_audio_manager_loaded_object(
    AUDIO_MANAGER_TEMPLATE_SOURCE_FILE, instrument_path,
    instrument_name, object_key, voice_count);

  if(audio_object != 0)
    {
      index = music_library_globals.loaded_audio_object_count;
      music_library_globals.loaded_audio_object_count = index + 1;
      gLoadedAudioObjects[index] = audio_object;
    }

  return audio_object;
}

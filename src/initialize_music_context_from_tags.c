#include "string.h"
/* Reconstructed from the original function at 0x0003D21C. */



#include "audio_manager_collection_runtime.h"
#include "audio_manager_voice_internal.h"
#include "audio_mixer_runtime.h"
#include "path_runtime.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"

int
find_music_tag_value(int  tag_list,
                     int  target_tag,
                     int *out_value,
                     int  start_index);
int
read_tag_argument_at_index(int *tag_list,
                           int  index,
                           int *out_kind,
                           int *out_item);
int
enter_audio_working_directory(char *path,
                              int   capacity);
int
change_music_library_directory(char *path);

typedef struct MusicNamedInstrumentEntry
{
  int voice_count;
  const char *instrument_name;
} MusicNamedInstrumentEntry;

enum MusicContextTag
{
  MUSIC_CONTEXT_MIXER_INPUT_COUNT_TAG = 9,
  MUSIC_CONTEXT_SECONDARY_MIXER_TAG = 10
};

enum MusicContextEntryKind
{
  MUSIC_CONTEXT_ENTRY_SAMPLER = 1,
  MUSIC_CONTEXT_ENTRY_DCSQXD_MONO = 2,
  MUSIC_CONTEXT_ENTRY_DCSQXD_HALF_MONO = 3,
  MUSIC_CONTEXT_ENTRY_FIXED_MONO_8 = 4,
  MUSIC_CONTEXT_ENTRY_VARIABLE_MONO_8 = 5,
  MUSIC_CONTEXT_ENTRY_NAMED_INSTRUMENT = 6,
  MUSIC_CONTEXT_ENTRY_SAMPLE_IMAGE = 7,
  MUSIC_CONTEXT_ENTRY_AUDIO_COLLECTION = 8,
  MUSIC_CONTEXT_ENTRY_SECONDARY_MIXER = 9,
  MUSIC_CONTEXT_ENTRY_END = 10
};

enum MusicContextConstant
{
  MUSIC_CONTEXT_NAME_CAPACITY = 256,
  MUSIC_CONTEXT_ERROR_COLLECTION_LOAD = -31,
  MUSIC_CONTEXT_ERROR_OBJECT_LOAD = -19,
  MUSIC_CONTEXT_ERROR_UNKNOWN_ENTRY = -20,
  MUSIC_CONTEXT_MAX_MIXER_LEVEL = 0x7FFF
};

int
initialize_music_context_from_tags(int tag_list)
{
  int item;
  int kind;
  int result;
  int index;
  MusicNamedInstrumentEntry *entry;
  int generated_kind;
  char name[MUSIC_CONTEXT_NAME_CAPACITY];
  char temp[MUSIC_CONTEXT_NAME_CAPACITY];

  item = 0;
  kind = 0;
  result = 0;
  index = 0;
  temp[0] = 0;

  stop_and_clear_all_audio_manager_loaded_object_voices();
  destroy_all_audio_manager_loaded_objects();
  music_library_globals.secondary_mixer_enabled = 1;

  if(find_music_tag_value(tag_list, MUSIC_CONTEXT_SECONDARY_MIXER_TAG,
                          &item, 0) != 0)
    {
      music_library_globals.secondary_mixer_enabled =
        (unsigned char)item;
    }

  if(find_music_tag_value(tag_list, MUSIC_CONTEXT_MIXER_INPUT_COUNT_TAG,
                          &item, 0) != 0)
    {
      result = configure_global_audio_mixer(item, 0);
      if(result < 0)
        {
          if(music_library_globals.last_error == 0)
            {
              music_library_globals.last_error =
                MUSIC_CONTEXT_ERROR_COLLECTION_LOAD;
            }
          result = music_library_globals.last_error;
          goto done;
        }
    }

  for(;;)
    {
      if(tag_list == 0)
        {
          goto done;
        }
      if(read_tag_argument_at_index((int *)tag_list, index,
                                    &kind, &item) == 0)
        {
          goto done;
        }

      switch(kind)
        {
        case MUSIC_CONTEXT_ENTRY_SAMPLER:
        case MUSIC_CONTEXT_ENTRY_DCSQXD_MONO:
        case MUSIC_CONTEXT_ENTRY_DCSQXD_HALF_MONO:
        case MUSIC_CONTEXT_ENTRY_FIXED_MONO_8:
        case MUSIC_CONTEXT_ENTRY_VARIABLE_MONO_8:
          if(item > 0)
            {
              enter_audio_working_directory(
                temp, MUSIC_CONTEXT_NAME_CAPACITY);
              switch(kind)
                {
                case MUSIC_CONTEXT_ENTRY_SAMPLER:
                  memcpy(name, "sampler.dsp", sizeof("sampler.dsp"));
                  break;
                case MUSIC_CONTEXT_ENTRY_DCSQXD_MONO:
                  memcpy(name, "dcsqxdmono.dsp",
                         sizeof("dcsqxdmono.dsp"));
                  break;
                case MUSIC_CONTEXT_ENTRY_DCSQXD_HALF_MONO:
                  memcpy(name, "dcsqxdhalfmono.dsp",
                         sizeof("dcsqxdhalfmono.dsp"));
                  break;
                case MUSIC_CONTEXT_ENTRY_FIXED_MONO_8:
                  memcpy(name, "fixedmono8.dsp",
                         sizeof("fixedmono8.dsp"));
                  break;
                case MUSIC_CONTEXT_ENTRY_VARIABLE_MONO_8:
                  memcpy(name, "varmono8.dsp", sizeof("varmono8.dsp"));
                  break;
                default:
                  memcpy(name, "sampler.dsp", sizeof("sampler.dsp"));
                  break;
                }

              if(load_audio_manager_object_from_file(
                   name, item, kind) == 0)
                {
                  result = music_library_globals.last_error;
                  if(result >= 0)
                    {
                      result = MUSIC_CONTEXT_ERROR_OBJECT_LOAD;
                    }
                  goto done;
                }
            }
          break;

        case MUSIC_CONTEXT_ENTRY_NAMED_INSTRUMENT:
          enter_audio_working_directory(temp, MUSIC_CONTEXT_NAME_CAPACITY);
          entry = (MusicNamedInstrumentEntry *)item;
          if(copy_path_with_capacity(
               name, sizeof(name), entry->instrument_name) < 0)
            {
              result = PATH_RUNTIME_ERROR_TOO_LONG;
              goto done;
            }
          generated_kind = next_audio_manager_object_key();
          if(load_audio_manager_object_from_file(
               name, entry->voice_count, generated_kind) == 0)
            {
              result = music_library_globals.last_error;
              if(result >= 0)
                {
                  result = MUSIC_CONTEXT_ERROR_OBJECT_LOAD;
                }
              goto done;
            }
          break;

        case MUSIC_CONTEXT_ENTRY_SAMPLE_IMAGE:
          result = load_audio_manager_collection_from_byte_file(
            (AudioManagerCollectionFileSource *)item, 0);
          if(result < 0)
            {
              goto done;
            }
          break;

        case MUSIC_CONTEXT_ENTRY_AUDIO_COLLECTION:
          result = parse_audio_manager_collection_image(
            (const void *)item, 0);
          if(result < 0)
            {
              goto done;
            }
          break;

        case MUSIC_CONTEXT_ENTRY_SECONDARY_MIXER:
        case MUSIC_CONTEXT_ENTRY_END:
          break;

        default:
          result = MUSIC_CONTEXT_ERROR_UNKNOWN_ENTRY;
          goto done;
        }

      if(result < 0)
        {
          goto done;
        }
      index++;
    }

done:
  if(temp[0] != 0)
    {
      change_music_library_directory(temp);
    }

  if(music_library_globals.mixer_enabled != 0 &&
     music_library_globals.secondary_mixer_enabled != 0)
    {
      set_audio_mixer_enabled(music_library_globals.global_mixer, 0);
      set_audio_mixer_maximum_amplitude(
        music_library_globals.global_mixer,
        signed_divide_with_remainder(
          music_library_globals.live_sound_count,
          MUSIC_CONTEXT_MAX_MIXER_LEVEL).quotient);
    }

  return result;
}

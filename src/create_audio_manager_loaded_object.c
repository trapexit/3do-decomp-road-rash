#include "string.h"
#include "limits.h"

#include "audio.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

char *
copy_audio_working_directory(char *path);

typedef struct AudioManagerTemplateTagList
{
  RwTagArgument entries[5];
  unsigned int terminator;
} AudioManagerTemplateTagList;

enum AudioManagerLoadedObjectConstant
{
  AUDIO_MANAGER_TEMPLATE_PATH_CAPACITY = 256
};

enum AudioManagerTemplateTagIndex
{
  AUDIO_MANAGER_IMAGE_ADDRESS_TAG_INDEX = 0,
  AUDIO_MANAGER_IMAGE_LENGTH_TAG_INDEX = 1,
  AUDIO_MANAGER_TEMPLATE_NAME_TAG_INDEX = 2,
  AUDIO_MANAGER_ALLOCATOR_TAG_INDEX = 3,
  AUDIO_MANAGER_RELEASER_TAG_INDEX = 4
};

AudioManagerLoadedObject *
create_audio_manager_loaded_object(int         template_source_kind,
                                   const void *template_source,
                                   const char *instrument_name,
                                   int         object_key,
                                   int         voice_count)
{
  AudioManagerLoadedObject *audio_object;
  const AudioManagerInstrumentTemplateImage *template_image;
  AudioManagerTemplateTagList template_tags;
  AudioManagerVoice *voice;
  RwMemoryAllocator allocate;
  int template_item;
  int result;
  int index;
  int name_length;
  size_t instrument_name_length;
  char path[AUDIO_MANAGER_TEMPLATE_PATH_CAPACITY];

  allocate = music_library_globals.allocate;
  if(allocate == 0 || instrument_name == 0)
    {
      music_library_globals.last_error = AUDIO_MANAGER_BAD_OBJECT_ERROR;
      return 0;
    }
  audio_object = (AudioManagerLoadedObject *)allocate(
    sizeof(AudioManagerLoadedObject), 0);
  if(audio_object == 0)
    {
      music_library_globals.last_error = AUDIO_MANAGER_NO_MEMORY_ERROR;
      return 0;
    }

  audio_object->instrument_template_item = -1;
  audio_object->lookup_flags = 0;
  audio_object->decoder_retained = 0;
  audio_object->instrument_name = 0;
  audio_object->object_key = object_key;
  audio_object->voice_count = 0;
  audio_object->voice_search_start = 0;

  for(index = 0; index < AUDIO_MANAGER_OBJECT_VOICE_CAPACITY; index++)
    {
      audio_object->voices[index] = 0;
    }

  if(voice_count <= 0 ||
     voice_count > AUDIO_MANAGER_OBJECT_VOICE_CAPACITY)
    {
      result = AUDIO_MANAGER_INVALID_VOICE_COUNT_ERROR;
      goto fail;
    }
  audio_object->voice_count = voice_count;

  instrument_name_length = strlen(instrument_name);
  if(instrument_name_length > (size_t)(INT_MAX - 1))
    {
      result = AUDIO_MANAGER_BAD_OBJECT_ERROR;
      goto fail;
    }
  name_length = (int)instrument_name_length;
  audio_object->instrument_name =
    (char *)allocate(name_length + 1, 0);
  if(audio_object->instrument_name == 0)
    {
      result = AUDIO_MANAGER_NO_MEMORY_ERROR;
      goto fail;
    }
  strcpy(audio_object->instrument_name, instrument_name);

  result = retain_audio_manager_adpcm_decoder(audio_object);
  if(result < 0)
    {
      goto fail;
    }
  audio_object->decoder_retained =
    (unsigned char)is_audio_manager_adpcm_instrument(
      audio_object->instrument_name);

  if(template_source_kind == AUDIO_MANAGER_TEMPLATE_SOURCE_IMAGE ||
     template_source_kind == AUDIO_MANAGER_TEMPLATE_SOURCE_FILE)
    {
      result = -1;

      if(template_source_kind == AUDIO_MANAGER_TEMPLATE_SOURCE_IMAGE)
        {
          template_image =
            (const AudioManagerInstrumentTemplateImage *)template_source;
          if(template_image == 0)
            {
              result = AUDIO_MANAGER_BAD_OBJECT_ERROR;
              goto fail;
            }

          template_tags.entries[
            AUDIO_MANAGER_IMAGE_ADDRESS_TAG_INDEX].tag =
            AF_TAG_IMAGE_ADDRESS;
          template_tags.entries[
            AUDIO_MANAGER_IMAGE_ADDRESS_TAG_INDEX].argument =
            (unsigned int)template_image->image_address;
          template_tags.entries[
            AUDIO_MANAGER_IMAGE_LENGTH_TAG_INDEX].tag =
            AF_TAG_IMAGE_LENGTH;
          template_tags.entries[
            AUDIO_MANAGER_IMAGE_LENGTH_TAG_INDEX].argument =
            (unsigned int)template_image->image_length;
          template_tags.entries[
            AUDIO_MANAGER_TEMPLATE_NAME_TAG_INDEX].tag = AF_TAG_NAME;
          template_tags.entries[
            AUDIO_MANAGER_TEMPLATE_NAME_TAG_INDEX].argument =
            (unsigned int)template_image->instrument_name;
          template_tags.entries[
            AUDIO_MANAGER_ALLOCATOR_TAG_INDEX].tag =
            AF_TAG_ALLOC_FUNCTION;
          template_tags.entries[
            AUDIO_MANAGER_ALLOCATOR_TAG_INDEX].argument =
            (unsigned int)music_library_globals.allocate;
          template_tags.entries[
            AUDIO_MANAGER_RELEASER_TAG_INDEX].tag =
            AF_TAG_FREE_FUNCTION;
          template_tags.entries[
            AUDIO_MANAGER_RELEASER_TAG_INDEX].argument =
            (unsigned int)music_library_globals.release;
          template_tags.terminator = 0;

          result = CreateInsTemplate((TagArg *)&template_tags);
          if(result < 0)
            {
              goto fail;
            }
        }
      else
        {
          if(template_source == 0)
            {
              result = AUDIO_MANAGER_BAD_OBJECT_ERROR;
              goto fail;
            }

          strcpy(path, instrument_name);
          result = LoadInsTemplate(path, 0);
          if(result < 0)
            {
              copy_audio_working_directory(path);
              strcat(path, "/");
              strcat(path, (const char *)template_source);
              result = LoadInsTemplate(path, 0);
            }
          if(result < 0)
            {
              copy_audio_working_directory(path);
              strcat(path, "/ins/");
              strcat(path, (const char *)template_source);
              result = LoadInsTemplate(path, 0);
            }
          if(result < 0)
            {
              goto fail;
            }
        }

      template_source_kind = AUDIO_MANAGER_TEMPLATE_SOURCE_ITEM;
      audio_object->instrument_template_item = result;
      template_source = (const void *)result;
    }

  for(index = 0; index < audio_object->voice_count; index++)
    {
      template_item = (int)template_source;
      voice = create_audio_manager_voice_record(
        template_source_kind, template_item);
      audio_object->voices[index] = voice;
      if(voice == 0)
        {
          result = music_library_globals.last_error;
          goto fail;
        }
    }

  return audio_object;

fail:
  destroy_audio_manager_loaded_object(audio_object);
  music_library_globals.last_error = result;
  return 0;
}

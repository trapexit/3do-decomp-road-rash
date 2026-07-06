#include "string.h"

#include "audio_manager_collection_runtime.h"
#include "rw_semantic_data.h"

int
parse_audio_manager_collection_entry(const void                *entry_image,
                                     AudioManagerLoadedObject **out_object)
{
  const unsigned char *cursor;
  const unsigned char *end;
  const unsigned char *instrument_form;
  AudioManagerLoadedObject *audio_object;
  AudioManagerInstrumentTemplateImage template_image;
  IffFormHeader entry_header;
  IffChunkHeader chunk_header;
  IffFormHeader form_header;
  unsigned int chunk_size;
  unsigned char name_length;
  int instrument_form_size;
  int result;
  int voice_count;
  char instrument_name[AUDIO_MANAGER_COLLECTION_NAME_CAPACITY];

  result = 0;
  audio_object = 0;
  instrument_form = 0;
  instrument_form_size = 0;
  voice_count = 0;

  memcpy(&entry_header, entry_image, IFF_FORM_HEADER_SIZE);
  chunk_size = (unsigned int)entry_header.chunk_size;
  end = (const unsigned char *)entry_image + chunk_size +
        (chunk_size & IFF_CHUNK_ALIGNMENT_MASK) + IFF_CHUNK_HEADER_SIZE;
  cursor = (const unsigned char *)entry_image + IFF_FORM_HEADER_SIZE;

  while(cursor < end)
    {
      memcpy(&chunk_header, cursor, IFF_CHUNK_HEADER_SIZE);
      chunk_size = (unsigned int)chunk_header.chunk_size;

      if(chunk_header.chunk_id == IFF_FORM_CHUNK_ID)
        {
          memcpy(&form_header, cursor, IFF_FORM_HEADER_SIZE);
          if(form_header.form_type == MUSIC_INSTRUMENT_TEMPLATE_FORM_ID)
            {
              instrument_form_size = form_header.chunk_size +
                                     IFF_CHUNK_HEADER_SIZE;
              instrument_form = cursor;
            }
        }
      else if(chunk_header.chunk_id ==
              AUDIO_MANAGER_COLLECTION_ENTRY_INFO_ID)
        {
          name_length = 0;
          memcpy(&voice_count,
                 cursor + IFF_CHUNK_HEADER_SIZE +
                 AUDIO_MANAGER_ENTRY_VOICE_COUNT_OFFSET,
                 sizeof(voice_count));
          memcpy(&name_length,
                 cursor + IFF_CHUNK_HEADER_SIZE +
                 AUDIO_MANAGER_ENTRY_NAME_LENGTH_OFFSET,
                 sizeof(name_length));
          memcpy(instrument_name,
                 cursor + IFF_CHUNK_HEADER_SIZE +
                 AUDIO_MANAGER_ENTRY_NAME_OFFSET,
                 name_length);
          instrument_name[name_length] = 0;
        }

      cursor += chunk_size + (chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
                IFF_CHUNK_HEADER_SIZE;
    }

  if(instrument_form != 0)
    {
      if(instrument_form_size <= 0)
        {
          voice_count = instrument_form_size;
        }
      if(voice_count > 0)
        {
          template_image.image_address = instrument_form;
          template_image.image_length = instrument_form_size;
          template_image.instrument_name = instrument_name;
          audio_object = create_audio_manager_object_from_template_image(
            &template_image, voice_count,
            next_audio_manager_object_key());
          if(audio_object == 0)
            {
              result = music_library_globals.last_error;
              if(result == 0)
                {
                  return AUDIO_MANAGER_COLLECTION_OBJECT_ERROR;
                }
              return result;
            }
        }
    }

  *out_object = audio_object;
  return result;
}

#include "string.h"

#include "audio_manager_collection_runtime.h"

int
DLL_AddTail(DLLList *list,
            int      value);

int
parse_audio_manager_collection_image(const void *collection_image,
                                     DLLList    *loaded_objects)
{
  const unsigned char *cursor;
  const unsigned char *end;
  AudioManagerLoadedObject *audio_object;
  IffFormHeader collection_header;
  IffChunkHeader chunk_header;
  IffFormHeader form_header;
  unsigned int chunk_size;
  int result;

  result = 0;
  audio_object = 0;

  memcpy(&collection_header, collection_image, IFF_FORM_HEADER_SIZE);
  chunk_size = (unsigned int)collection_header.chunk_size;
  end = (const unsigned char *)collection_image + chunk_size +
        (chunk_size & IFF_CHUNK_ALIGNMENT_MASK) + IFF_CHUNK_HEADER_SIZE;
  cursor = (const unsigned char *)collection_image + IFF_FORM_HEADER_SIZE;
  if(cursor < end)
    {
      for(;;)
        {
          memcpy(&chunk_header, cursor, IFF_CHUNK_HEADER_SIZE);
          chunk_size = (unsigned int)chunk_header.chunk_size;
          if(chunk_header.chunk_id == IFF_FORM_CHUNK_ID)
            {
              memcpy(&form_header, cursor, IFF_FORM_HEADER_SIZE);
              if(form_header.form_type ==
                 AUDIO_MANAGER_COLLECTION_ENTRY_FORM_ID)
                {
                  result = parse_audio_manager_collection_entry(
                    cursor, &audio_object);
                  if(result != 0)
                    {
                      return result;
                    }
                }
            }

          cursor += chunk_size +
                    (chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
                    IFF_CHUNK_HEADER_SIZE;
          if(loaded_objects != 0)
            {
              result = DLL_AddTail(loaded_objects, (int)audio_object);
              if(result != 0)
                {
                  return result;
                }
            }
          if(end <= cursor)
            {
              return 0;
            }
        }
    }

  return result;
}

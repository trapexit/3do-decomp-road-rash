#include "audio_manager_collection_runtime.h"
#include "rw_semantic_data.h"

int
load_audio_manager_collection_from_byte_file(const AudioManagerCollectionFileSource *source,
                                             DLLList                                *loaded_objects)
{
  IffFormHeader form_header;
  void *collection_image;
  int image_size;
  int result;
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;

  if(source == 0 || source->reader == 0 || source->offset < 0)
    {
      return AUDIO_MANAGER_BAD_OBJECT_ERROR;
    }

  result = BF_Seek(source->reader, source->offset, BYTE_FILE_SEEK_SET);
  if(result < 0)
    {
      return result;
    }

  result = IFF_FindForm(source->reader, AUDIO_MANAGER_COLLECTION_FORM_ID,
                        -1, &form_header);
  if(result < 0)
    {
      return result;
    }

  image_size = form_header.chunk_size +
               (form_header.chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
               IFF_CHUNK_HEADER_SIZE;
  allocate = music_library_globals.allocate;
  collection_image = (void *)allocate(image_size, 0);
  if(collection_image == 0)
    {
      return ROADRASH_MUSIC_NO_MEMORY_ERROR;
    }

  result = read_music_file_bytes(source->reader, collection_image,
                                 image_size);
  if(result >= 0)
    {
      result = parse_audio_manager_collection_image(
        collection_image, loaded_objects);
    }

  release = music_library_globals.release;
  release((int)collection_image, image_size);

  return result;
}

#include "audio.h"
#include "mem.h"

#include "music_file_runtime.h"
#include "rw_semantic_data.h"

enum InstrumentTemplateTagIndex
{
  INSTRUMENT_TEMPLATE_IMAGE_ADDRESS_TAG = 0,
  INSTRUMENT_TEMPLATE_IMAGE_LENGTH_TAG,
  INSTRUMENT_TEMPLATE_ALLOCATOR_TAG,
  INSTRUMENT_TEMPLATE_RELEASER_TAG,
  INSTRUMENT_TEMPLATE_END_TAG,
  INSTRUMENT_TEMPLATE_TAG_COUNT
};

int
load_instrument_template_from_byte_file(ByteFileReader *reader,
                                        int             start_position)
{
  TagArg template_tags[INSTRUMENT_TEMPLATE_TAG_COUNT];
  IffFormHeader form_header;
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
  int result;
  int template_item;
  int image_bytes;
  int image;

  template_item = -1;
  image = 0;

  if(start_position >= 0)
    {
      result = BF_Seek(reader, start_position, BYTE_FILE_SEEK_SET);
      if(result < 0)
        {
          return result;
        }
    }

  result = IFF_PeekFormHeader(reader, &form_header);
  if(result != 0)
    {
      return result;
    }
  if(form_header.form_type != MUSIC_INSTRUMENT_TEMPLATE_FORM_ID)
    {
      return ROADRASH_MUSIC_BAD_FORMAT_ERROR;
    }

  image_bytes = form_header.chunk_size +
                (form_header.chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
                IFF_CHUNK_HEADER_SIZE;
  allocate = music_library_globals.allocate;
  image = allocate(image_bytes, MEMTYPE_AUDIO);
  if(image == 0)
    {
      return ROADRASH_MUSIC_NO_MEMORY_ERROR;
    }

  result = read_music_file_bytes(reader, (void *)image, image_bytes);
  if(result >= 0)
    {
      result = IFF_AlignReader(reader);
      if(result == 0)
        {
          template_tags[
            INSTRUMENT_TEMPLATE_IMAGE_ADDRESS_TAG].ta_Tag =
            AF_TAG_IMAGE_ADDRESS;
          template_tags[
            INSTRUMENT_TEMPLATE_IMAGE_ADDRESS_TAG].ta_Arg =
            (void *)image;
          template_tags[
            INSTRUMENT_TEMPLATE_IMAGE_LENGTH_TAG].ta_Tag =
            AF_TAG_IMAGE_LENGTH;
          template_tags[
            INSTRUMENT_TEMPLATE_IMAGE_LENGTH_TAG].ta_Arg =
            (void *)image_bytes;
          template_tags[
            INSTRUMENT_TEMPLATE_ALLOCATOR_TAG].ta_Tag =
            AF_TAG_ALLOC_FUNCTION;
          template_tags[
            INSTRUMENT_TEMPLATE_ALLOCATOR_TAG].ta_Arg =
            (void *)music_library_globals.allocate;
          template_tags[
            INSTRUMENT_TEMPLATE_RELEASER_TAG].ta_Tag =
            AF_TAG_FREE_FUNCTION;
          template_tags[
            INSTRUMENT_TEMPLATE_RELEASER_TAG].ta_Arg =
            (void *)music_library_globals.release;
          template_tags[INSTRUMENT_TEMPLATE_END_TAG].ta_Tag = TAG_END;
          template_tags[INSTRUMENT_TEMPLATE_END_TAG].ta_Arg = 0;

          template_item = CreateInsTemplate(template_tags);
          if(template_item < 0)
            {
              result = template_item;
            }
        }
    }

  release = music_library_globals.release;
  release(image, image_bytes);

  if(result != 0)
    {
      return result;
    }
  return template_item;
}

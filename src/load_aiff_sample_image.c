#include "audio.h"
#include "mem.h"
#include "string.h"

#include "aiff_sample_runtime.h"
#include "rw_semantic_data.h"

enum AiffImageSampleTagIndex
{
  AIFF_IMAGE_TAG_NUMBYTES = 0,
  AIFF_IMAGE_TAG_ADDRESS,
  AIFF_IMAGE_TAG_END,
  AIFF_IMAGE_TAG_COUNT
};

static
int
aiff_image_chunk_storage_bytes(int chunk_size)
{
  return chunk_size + (chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
         IFF_CHUNK_HEADER_SIZE;
}


static
unsigned char *
find_aiff_image_chunk(unsigned char *chunk,
                      unsigned char *end,
                      unsigned int   wanted_id)
{
  IffChunkHeader header;
  int remaining;

  while(chunk < end)
    {
      remaining = (int)(end - chunk);
      if(remaining < IFF_CHUNK_HEADER_SIZE)
        {
          return 0;
        }
      memcpy(&header, chunk, sizeof(header));
      if(header.chunk_size < 0 ||
         header.chunk_size > remaining - IFF_CHUNK_HEADER_SIZE ||
         aiff_image_chunk_storage_bytes(header.chunk_size) > remaining)
        {
          return 0;
        }
      if(header.chunk_id == wanted_id)
        {
          return chunk;
        }
      chunk += aiff_image_chunk_storage_bytes(header.chunk_size);
    }
  return 0;
}


int
load_aiff_sample_image(void *sample_image,
                       int   copy_sample_data)
{
  TagArg sample_tags[AIFF_IMAGE_TAG_COUNT];
  IffFormHeader *form;
  IffChunkHeader sound_header;
  AiffSoundDataHeader sound_data_header;
  unsigned char *image;
  unsigned char *first_chunk;
  unsigned char *image_end;
  unsigned char *common_chunk;
  unsigned char *sound_chunk;
  unsigned char *marker_chunk;
  unsigned char *instrument_chunk;
  int sample_data;
  int sample_data_bytes;
  int sample_item;
  int result;

  copy_sample_data = (unsigned char)copy_sample_data;
  sample_item = -1;
  common_chunk = 0;
  marker_chunk = 0;
  sound_chunk = 0;
  sample_data = 0;
  sample_data_bytes = 0;
  instrument_chunk = 0;

  if(sample_image == 0)
    {
      return ROADRASH_MUSIC_BAD_FORMAT_ERROR;
    }

  image = (unsigned char *)sample_image;
  form = (IffFormHeader *)image;
  if(form->chunk_size < IFF_FORM_TYPE_SIZE ||
     form->chunk_size > 0x7FFFFFFF - IFF_CHUNK_HEADER_SIZE)
    {
      return ROADRASH_MUSIC_BAD_FORMAT_ERROR;
    }
  first_chunk = image + IFF_FORM_HEADER_SIZE;
  image_end = image + form->chunk_size + IFF_CHUNK_HEADER_SIZE;

  common_chunk = find_aiff_image_chunk(
    first_chunk, image_end, AIFF_COMMON_CHUNK_ID);
  sound_chunk = find_aiff_image_chunk(
    first_chunk, image_end, AIFF_SOUND_DATA_CHUNK_ID);
  marker_chunk = find_aiff_image_chunk(
    first_chunk, image_end, AIFF_MARKER_CHUNK_ID);
  instrument_chunk = find_aiff_image_chunk(
    first_chunk, image_end, AIFF_INSTRUMENT_CHUNK_ID);

  if(common_chunk == 0 || sound_chunk == 0)
    {
      result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
      music_library_globals.last_error = result;
    }
  else
    {
      memcpy(&sound_header, sound_chunk, sizeof(sound_header));
      if(sound_header.chunk_size < sizeof(sound_data_header))
        {
          result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
          goto cleanup;
        }
      memcpy(&sound_data_header,
             sound_chunk + IFF_CHUNK_HEADER_SIZE,
             sizeof(sound_data_header));
      if(sound_data_header.sample_offset < 0 ||
         sound_data_header.sample_offset >
           sound_header.chunk_size - sizeof(sound_data_header))
        {
          result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
          goto cleanup;
        }

      sample_data_bytes = sound_header.chunk_size -
                          sizeof(sound_data_header) - sound_data_header.sample_offset;
      if(copy_sample_data != 0)
        {
          sample_data = music_library_globals.allocate(
            sample_data_bytes, MEMTYPE_AUDIO);
          if(sample_data == 0)
            {
              result = ROADRASH_MUSIC_NO_MEMORY_ERROR;
              music_library_globals.last_error = result;
              goto cleanup;
            }
          memcpy((void *)sample_data,
                 sound_chunk + IFF_CHUNK_HEADER_SIZE +
                 sizeof(sound_data_header) +
                 sound_data_header.sample_offset,
                 sample_data_bytes);
        }
      else
        {
          sample_data = (int)(sound_chunk + IFF_CHUNK_HEADER_SIZE +
                              sizeof(sound_data_header) +
                              sound_data_header.sample_offset);
        }

      sample_item = create_aiff_sample(
        form->form_type, common_chunk, marker_chunk, instrument_chunk);
      if(sample_item < 0)
        {
          result = sample_item;
          music_library_globals.last_error = result;
        }
      else
        {
          sample_tags[AIFF_IMAGE_TAG_NUMBYTES].ta_Tag = AF_TAG_NUMBYTES;
          sample_tags[AIFF_IMAGE_TAG_NUMBYTES].ta_Arg =
            (void *)sample_data_bytes;
          sample_tags[AIFF_IMAGE_TAG_ADDRESS].ta_Tag = AF_TAG_ADDRESS;
          sample_tags[AIFF_IMAGE_TAG_ADDRESS].ta_Arg =
            (void *)sample_data;
          sample_tags[AIFF_IMAGE_TAG_END].ta_Tag = TAG_END;
          sample_tags[AIFF_IMAGE_TAG_END].ta_Arg = 0;

          result = SetAudioItemInfo(sample_item, sample_tags);
          if(result < 0)
            {
              music_library_globals.last_error = result;
            }
          if(result == 0)
            {
              return sample_item;
            }
        }
    }

cleanup:
  if(sample_item >= 0)
    {
      UnloadSample(sample_item);
    }
  if(sample_data != 0 && copy_sample_data != 0)
    {
      music_library_globals.release(sample_data, sample_data_bytes);
    }
  sample_item = -1;
  if(result < 0)
    {
      return result;
    }
  return sample_item;
}

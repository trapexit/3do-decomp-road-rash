#include "audio.h"
#include "mem.h"

#include "aiff_sample_runtime.h"
#include "rw_semantic_data.h"

enum AiffSampleLoadTagIndex
{
  AIFF_SAMPLE_TAG_NUMBYTES = 0,
  AIFF_SAMPLE_TAG_ADDRESS,
  AIFF_SAMPLE_TAG_END,
  AIFF_SAMPLE_TAG_COUNT
};

static
int
aiff_chunk_storage_bytes(int chunk_size)
{
  return chunk_size + (chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
         IFF_CHUNK_HEADER_SIZE;
}


static
int
is_aiff_metadata_chunk(unsigned int chunk_id)
{
  return chunk_id == AIFF_COMMON_CHUNK_ID ||
         chunk_id == AIFF_MARKER_CHUNK_ID ||
         chunk_id == AIFF_INSTRUMENT_CHUNK_ID;
}


static
void
release_aiff_metadata_chunk(int chunk_data)
{
  IffChunkHeader *header;

  if(chunk_data != 0)
    {
      header = (IffChunkHeader *)chunk_data;
      music_library_globals.release(
        chunk_data, header->chunk_size + IFF_CHUNK_HEADER_SIZE);
    }
}


int
load_aiff_sample_from_byte_file(ByteFileReader *reader,
                                int             start_position)
{
  TagArg sample_tags[AIFF_SAMPLE_TAG_COUNT];
  IffFormHeader form_header;
  IffChunkHeader chunk_header;
  AiffSoundDataHeader sound_data_header;
  int result;
  int sample_item;
  int form_payload_bytes;
  int scanned_bytes;
  int chunk_data;
  int common_chunk;
  int marker_chunk;
  int instrument_chunk;
  int sample_data;
  int sample_data_bytes;

  sample_item = -1;
  common_chunk = 0;
  marker_chunk = 0;
  instrument_chunk = 0;
  sample_data = 0;
  sample_data_bytes = 0;
  scanned_bytes = 0;
  result = -1;
  form_header.chunk_id = 0;
  form_header.chunk_size = 0;
  form_header.form_type = 0;
  chunk_header.chunk_id = 0;
  chunk_header.chunk_size = 0;
  sound_data_header.sample_offset = 0;
  sound_data_header.block_size = 0;

  if(start_position >= 0)
    {
      result = BF_Seek(reader, start_position, BYTE_FILE_SEEK_SET);
      if(result < 0)
        {
          goto done;
        }
    }

  result = BF_Read(reader, &form_header, IFF_FORM_HEADER_SIZE);
  if(result != IFF_FORM_HEADER_SIZE)
    {
      result = result < 0 ? result : ROADRASH_MUSIC_BAD_FORMAT_ERROR;
      goto done;
    }

  if(form_header.chunk_size < IFF_FORM_TYPE_SIZE ||
     form_header.chunk_size - IFF_FORM_TYPE_SIZE >
       (int)reader->file_status.fs_ByteCount - reader->position)
    {
      result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
      goto done;
    }
  form_payload_bytes = form_header.chunk_size - IFF_FORM_TYPE_SIZE;
  if(form_payload_bytes <= 0)
    {
      result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
      goto done;
    }

  while(scanned_bytes < form_payload_bytes)
    {
      result = IFF_PeekChunkHeader(reader, &chunk_header);
      if(result < 0)
        {
          goto cleanup;
        }

      if(chunk_header.chunk_size < 0 ||
         form_payload_bytes - scanned_bytes < IFF_CHUNK_HEADER_SIZE ||
         chunk_header.chunk_size >
           form_payload_bytes - scanned_bytes - IFF_CHUNK_HEADER_SIZE ||
         aiff_chunk_storage_bytes(chunk_header.chunk_size) >
           form_payload_bytes - scanned_bytes)
        {
          result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
          goto cleanup;
        }

      scanned_bytes += aiff_chunk_storage_bytes(chunk_header.chunk_size);

      if(is_aiff_metadata_chunk(chunk_header.chunk_id))
        {
          chunk_data = music_library_globals.allocate(
            chunk_header.chunk_size + IFF_CHUNK_HEADER_SIZE, 0);
          result = chunk_data;
          if(chunk_data == 0)
            {
              result = -1;
              goto cleanup;
            }

          result = IFF_ReadChunk(
            reader, chunk_header.chunk_id, -1, chunk_data, 0);
          if(result == 0)
            {
              music_library_globals.release(
                chunk_data, chunk_header.chunk_size + IFF_CHUNK_HEADER_SIZE);
              result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
              goto cleanup;
            }

          if(chunk_header.chunk_id == AIFF_COMMON_CHUNK_ID)
            {
              release_aiff_metadata_chunk(common_chunk);
              common_chunk = chunk_data;
            }
          else if(chunk_header.chunk_id == AIFF_MARKER_CHUNK_ID)
            {
              release_aiff_metadata_chunk(marker_chunk);
              marker_chunk = chunk_data;
            }
          else if(chunk_header.chunk_id ==
                  AIFF_INSTRUMENT_CHUNK_ID)
            {
              release_aiff_metadata_chunk(instrument_chunk);
              instrument_chunk = chunk_data;
            }
        }
      else if(chunk_header.chunk_id == AIFF_SOUND_DATA_CHUNK_ID)
        {
          if(sample_data != 0 ||
             chunk_header.chunk_size < sizeof(sound_data_header))
            {
              result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
              goto cleanup;
            }
          result = enter_music_iff_chunk_payload(reader);
          if(result < 0)
            {
              goto cleanup;
            }

          result = BF_Read(reader, &sound_data_header,
                           sizeof(sound_data_header));
          if(result != sizeof(sound_data_header))
            {
              result = result < 0 ? result : ROADRASH_MUSIC_BAD_FORMAT_ERROR;
              goto cleanup;
            }

          sample_data_bytes = chunk_header.chunk_size -
                              sizeof(sound_data_header);
          if(sound_data_header.sample_offset < 0 ||
             sound_data_header.sample_offset > sample_data_bytes)
            {
              result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
              goto cleanup;
            }
          result = BF_Seek(reader, sound_data_header.sample_offset,
                           BYTE_FILE_SEEK_CURRENT);
          if(result < 0)
            {
              goto cleanup;
            }
          sample_data_bytes -= sound_data_header.sample_offset;
          sample_data = music_library_globals.allocate(
            sample_data_bytes, MEMTYPE_AUDIO);
          if(sample_data == 0)
            {
              result = -1;
              goto cleanup;
            }

          result = BF_Read(reader, (void *)sample_data,
                           sample_data_bytes);
          if(result != sample_data_bytes)
            {
              result = result < 0 ? result : ROADRASH_MUSIC_BAD_FORMAT_ERROR;
              goto cleanup;
            }

          result = IFF_AlignReader(reader);
          if(result != 0)
            {
              goto cleanup;
            }
        }
      else
        {
          result = IFF_SkipChunk(reader, &chunk_header);
          if(result != 0)
            {
              goto cleanup;
            }
        }
    }

  if(common_chunk == 0 || sample_data == 0)
    {
      result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
      goto cleanup;
    }

  sample_item = create_aiff_sample(
    form_header.form_type, (unsigned char *)common_chunk,
    (unsigned char *)marker_chunk, (unsigned char *)instrument_chunk);
  result = sample_item;
  if(sample_item >= 0)
    {
      sample_tags[AIFF_SAMPLE_TAG_NUMBYTES].ta_Tag = AF_TAG_NUMBYTES;
      sample_tags[AIFF_SAMPLE_TAG_NUMBYTES].ta_Arg =
        (void *)sample_data_bytes;
      sample_tags[AIFF_SAMPLE_TAG_ADDRESS].ta_Tag = AF_TAG_ADDRESS;
      sample_tags[AIFF_SAMPLE_TAG_ADDRESS].ta_Arg = (void *)sample_data;
      sample_tags[AIFF_SAMPLE_TAG_END].ta_Tag = TAG_END;
      sample_tags[AIFF_SAMPLE_TAG_END].ta_Arg = 0;
      result = SetAudioItemInfo(sample_item, sample_tags);
    }

cleanup:
  release_aiff_metadata_chunk(common_chunk);
  release_aiff_metadata_chunk(marker_chunk);
  release_aiff_metadata_chunk(instrument_chunk);

  if(result < 0)
    {
      if(sample_item >= 0)
        {
          UnloadSample(sample_item);
        }
      if(sample_data != 0)
        {
          music_library_globals.release(sample_data, sample_data_bytes);
        }
    }
done:
  if(result < 0)
    {
      return result;
    }
  return sample_item;
}

#include "audio.h"
#include "item.h"
#include "mem.h"
#include "string.h"

#include "aiff_sample_runtime.h"
#include "path_runtime.h"
#include "rw_semantic_data.h"

char *
copy_audio_working_directory(char *path);
int
change_music_library_directory(char *path);

enum AiffFileSampleLoadConstant
{
  AIFF_FILE_PATH_CAPACITY = 256,
  AIFF_FILE_SAMPLE_TAG_COUNT = 3
};

enum AiffFileSampleTagIndex
{
  AIFF_FILE_SAMPLE_BYTES_TAG = 0,
  AIFF_FILE_SAMPLE_ADDRESS_TAG,
  AIFF_FILE_SAMPLE_END_TAG
};

static
void
release_aiff_file_chunk(int chunk,
                        int allocation_bytes)
{
  RwMemoryReleaser release;

  if(chunk != 0)
    {
      release = music_library_globals.release;
      release(chunk, allocation_bytes);
    }
}


int
load_aiff_sample_from_file(const char *sample_name)
{
  char path[AIFF_FILE_PATH_CAPACITY];
  volatile char restore_directory[AIFF_FILE_PATH_CAPACITY];
  TagArg sample_tags[AIFF_FILE_SAMPLE_TAG_COUNT];
  ByteFileReader *reader;
  int common_chunk;
  int common_chunk_bytes;
  int marker_chunk;
  int marker_chunk_bytes;
  int instrument_chunk;
  int instrument_chunk_bytes;
  unsigned int form_type;
  int sample_bytes;
  int sample_data;
  int sample_item;
  int result;
  RwMemoryAllocator allocate;

  sample_item = -1;
  result = -1;
  reader = 0;
  common_chunk = 0;
  common_chunk_bytes = 0;
  marker_chunk = 0;
  marker_chunk_bytes = 0;
  instrument_chunk = 0;
  instrument_chunk_bytes = 0;
  form_type = 0;
  restore_directory[0] = 0;
  sample_data = 0;
  sample_bytes = 0;

  copy_audio_working_directory(path);
  if(append_path_with_capacity(path, sizeof(path), "/AIFF/") < 0 ||
     append_path_with_capacity(path, sizeof(path), sample_name) < 0)
    {
      result = PATH_RUNTIME_ERROR_TOO_LONG;
      goto cleanup;
    }
  reader = BF_Open(path, 0);

  if(reader == 0)
    {
      copy_audio_working_directory(path);
      if(append_path_with_capacity(path, sizeof(path), "/") < 0 ||
         append_path_with_capacity(path, sizeof(path), sample_name) < 0)
        {
          result = PATH_RUNTIME_ERROR_TOO_LONG;
          goto cleanup;
        }
      reader = BF_Open(path, 0);
    }
  if(reader == 0)
    {
      if(copy_path_with_capacity(
           path, sizeof(path), sample_name) < 0)
        {
          result = PATH_RUNTIME_ERROR_TOO_LONG;
          goto cleanup;
        }
      reader = BF_Open(path, 0);
    }
  if(reader == 0)
    {
      if(copy_path_with_capacity(path, sizeof(path), "aiff/") < 0 ||
         append_path_with_capacity(path, sizeof(path), sample_name) < 0)
        {
          result = PATH_RUNTIME_ERROR_TOO_LONG;
          goto cleanup;
        }
      reader = BF_Open(path, 0);
    }

  if(reader != 0)
    {
      common_chunk = IFF_ReadAIFFChunk(
        reader, AIFF_COMMON_CHUNK_ID, 0, 0,
        &common_chunk_bytes, &form_type);
      if(common_chunk != 0)
        {
          marker_chunk = IFF_ReadAIFFChunk(
            reader, AIFF_MARKER_CHUNK_ID, 0, 0,
            &marker_chunk_bytes, 0);
          instrument_chunk = IFF_ReadAIFFChunk(
            reader, AIFF_INSTRUMENT_CHUNK_ID, 0, 0,
            &instrument_chunk_bytes, 0);

          sample_bytes = IFF_FindAIFFSampleData(reader, 0, 0, 0);
          if(sample_bytes < 0)
            {
              result = sample_bytes;
            }
          else
            {
              sample_item = create_aiff_sample(
                form_type, (unsigned char *)common_chunk,
                (unsigned char *)marker_chunk,
                (unsigned char *)instrument_chunk);
              result = sample_item;
              if(sample_item >= 0)
                {
                  sample_tags[AIFF_FILE_SAMPLE_BYTES_TAG].ta_Tag =
                    AF_TAG_NUMBYTES;
                  sample_tags[AIFF_FILE_SAMPLE_BYTES_TAG].ta_Arg =
                    (void *)sample_bytes;
                  sample_tags[AIFF_FILE_SAMPLE_ADDRESS_TAG].ta_Tag =
                    AF_TAG_ADDRESS;

                  allocate = music_library_globals.allocate;
                  sample_data = allocate(sample_bytes, MEMTYPE_AUDIO);
                  if(sample_data == 0)
                    {
                      result = ROADRASH_MUSIC_NO_MEMORY_ERROR;
                      goto cleanup;
                    }
                  sample_tags[AIFF_FILE_SAMPLE_ADDRESS_TAG].ta_Arg =
                    (void *)sample_data;
                  sample_tags[AIFF_FILE_SAMPLE_END_TAG].ta_Tag = TAG_END;

                  if(sample_data != 0)
                    {
                      result = SetAudioItemInfo(sample_item, sample_tags);
                      if(result >= 0)
                        {
                          if(IFF_ReadAIFFSampleData(
                               reader, 0, (void *)sample_data, 0, 0) == 0)
                            {
                              result = ROADRASH_MUSIC_BAD_FORMAT_ERROR;
                            }
                        }
                    }
                }
            }
        }
    }

cleanup:
  if(restore_directory[0] != 0)
    {
      change_music_library_directory((char *)restore_directory);
    }
  release_aiff_file_chunk(common_chunk, common_chunk_bytes);
  release_aiff_file_chunk(marker_chunk, marker_chunk_bytes);
  release_aiff_file_chunk(instrument_chunk, instrument_chunk_bytes);
  if(reader != 0)
    {
      BF_Close(reader);
    }

  if(result < 0)
    {
      if(sample_item >= 0)
        {
          UnloadSample(sample_item);
        }
      if(sample_data != 0)
        {
          music_library_globals.release(sample_data, sample_bytes);
        }
      return result;
    }
  return sample_item;
}

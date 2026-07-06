#ifndef ROADRASH_AIFF_SAMPLE_RUNTIME_H
#define ROADRASH_AIFF_SAMPLE_RUNTIME_H

#include "music_file_runtime.h"

enum AiffFormatValue
{
  AIFF_COMMON_CHUNK_ID = 0x434F4D4D,
  AIFF_MARKER_CHUNK_ID = 0x4D41524B,
  AIFF_INSTRUMENT_CHUNK_ID = 0x494E5354,
  AIFF_SOUND_DATA_CHUNK_ID = 0x53534E44,
  AIFF_FORM_ID = 0x41494646,
  AIFC_FORM_ID = 0x41494643
};

typedef struct AiffSoundDataHeader
{
  int sample_offset;
  int block_size;
} AiffSoundDataHeader;

int
enter_music_iff_chunk_payload(ByteFileReader *reader);
int
load_aiff_sample_from_byte_file(ByteFileReader *reader,
                                int             start_position);
int
load_aiff_sample_image(void *sample_image,
                       int   copy_sample_data);
int
create_aiff_sample(int            form_type,
                   unsigned char *common_chunk,
                   unsigned char *marker_chunk,
                   unsigned char *instrument_chunk);
int
load_aiff_sample_from_file(const char *sample_name);
int
IFF_ReadAIFFChunk(ByteFileReader *reader,
                  unsigned int    chunk_id,
                  int             start_position,
                  void           *destination,
                  int            *chunk_allocation_bytes,
                  unsigned int   *out_form_type);
int
IFF_FindAIFFSampleData(ByteFileReader *reader,
                       int             start_position,
                       unsigned int   *out_form_type,
                       IffChunkHeader *out_sound_chunk_header);
void *
IFF_ReadAIFFSampleData(ByteFileReader *reader,
                       int             start_position,
                       void           *destination,
                       int            *out_sample_bytes,
                       unsigned int   *out_form_type);

#endif

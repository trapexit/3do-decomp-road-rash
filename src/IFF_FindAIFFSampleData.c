#include "aiff_sample_runtime.h"
#include "rw_semantic_data.h"

int
IFF_FindAIFFSampleData(ByteFileReader *reader,
                       int             start_position,
                       unsigned int   *out_form_type,
                       IffChunkHeader *out_sound_chunk_header)
{
  IffChunkHeader sound_chunk_header;
  IffFormHeader form_header;
  int result;
  int sample_bytes;

  sample_bytes = -1;

  result = IFF_FindForm(reader, AIFF_FORM_ID, start_position,
                        &form_header);
  if(result == 0)
    {
      if(out_form_type != 0)
        {
          *out_form_type = AIFF_FORM_ID;
        }
    }
  else
    {
      result = IFF_FindForm(reader, AIFC_FORM_ID, start_position,
                            &form_header);
      if(result != 0)
        {
          block_file_api_globals.last_error = result;
          return result;
        }
      if(out_form_type != 0)
        {
          *out_form_type = AIFC_FORM_ID;
        }
    }

  result = IFF_EnterForm(reader);
  if(result != 0)
    {
      block_file_api_globals.last_error = result;
      return result;
    }

  result = IFF_FindChunk(reader, AIFF_SOUND_DATA_CHUNK_ID, -1,
                         &sound_chunk_header);
  if(result != 0)
    {
      block_file_api_globals.last_error = result;
      return result;
    }

  if(out_sound_chunk_header != 0)
    {
      *out_sound_chunk_header = sound_chunk_header;
    }

  sample_bytes = sound_chunk_header.chunk_size -
                 sizeof(AiffSoundDataHeader);
  if(sample_bytes < 0)
    {
      block_file_api_globals.last_error = -1;
      return -1;
    }
  result = BF_Seek(reader,
                   IFF_CHUNK_HEADER_SIZE + sizeof(AiffSoundDataHeader),
                   BYTE_FILE_SEEK_CURRENT);
  if(result < 0)
    {
      block_file_api_globals.last_error = result;
      return result;
    }

  return sample_bytes;
}

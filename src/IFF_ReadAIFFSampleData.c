#include "aiff_sample_runtime.h"
#include "rw_semantic_data.h"

enum IffAiffSampleDataError
{
  IFF_AIFF_SAMPLE_DATA_ALLOCATION_ERROR = 0x1404
};

void *
IFF_ReadAIFFSampleData(ByteFileReader *reader,
                       int             start_position,
                       void           *destination,
                       int            *out_sample_bytes,
                       unsigned int   *out_form_type)
{
  void *sample_data;
  int result;
  int sample_bytes;

  sample_bytes = IFF_FindAIFFSampleData(
    reader, start_position, out_form_type, 0);
  if(sample_bytes < 0)
    {
      block_file_api_globals.last_error = sample_bytes;
      return 0;
    }

  sample_data = destination;
  if(sample_data == 0)
    {
      sample_data = (void *)block_file_api_globals.allocate(
        sample_bytes, 0);
      if(sample_data == 0)
        {
          result = IFF_AIFF_SAMPLE_DATA_ALLOCATION_ERROR;
          block_file_api_globals.last_error = result;
          return 0;
        }
    }

  result = BF_Read(reader, sample_data, sample_bytes);
  if(result != sample_bytes)
    {
      block_file_api_globals.last_error = result < 0 ? result : -1;
      if(destination == 0)
        {
          block_file_api_globals.release((int)sample_data, sample_bytes);
        }
      return 0;
    }

  if(out_sample_bytes != 0)
    {
      *out_sample_bytes = sample_bytes;
    }

  return sample_data;
}

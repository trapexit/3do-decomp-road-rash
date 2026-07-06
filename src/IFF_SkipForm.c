#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
IFF_SkipForm(ByteFileReader      *reader,
             const IffFormHeader *form_header)
{
  int result;

  result = BF_Seek(reader, form_header->chunk_size,
                   BYTE_FILE_SEEK_CURRENT);
  if(result < 0)
    {
      block_file_api_globals.last_error = result;
      return result;
    }

  return 0;
}

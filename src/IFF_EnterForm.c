
#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
IFF_EnterForm(ByteFileReader *reader)
{
  int result;

  result = BF_Seek(reader, IFF_FORM_HEADER_SIZE,
                   BYTE_FILE_SEEK_CURRENT);
  if(result < 0)
    {
      block_file_api_globals.last_error = result;
      return result;
    }

  return 0;
}

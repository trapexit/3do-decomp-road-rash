#include "iff_runtime.h"
#include "rw_semantic_data.h"

int
IFF_FindForm(ByteFileReader *reader,
             unsigned int    form_type,
             int             start_position,
             IffFormHeader  *header)
{
  IffChunkHeader chunk_header;
  int result;

  if(start_position >= 0)
    {
      result = BF_Seek(reader, start_position, BYTE_FILE_SEEK_SET);
      if(result < 0)
        {
          block_file_api_globals.last_error = result;
          return result;
        }
    }

  result = IFF_FindChunk(reader, IFF_FORM_CHUNK_ID,
                         start_position, &chunk_header);
  if(result == 0)
    {
      result = IFF_PeekFormHeader(reader, header);
      while(result == 0)
        {
          if(header->form_type == form_type)
            {
              return 0;
            }

          result = IFF_SkipChunk(reader, &chunk_header);
          if(result != 0)
            {
              break;
            }

          result = IFF_FindChunk(reader, IFF_FORM_CHUNK_ID, -1,
                                 &chunk_header);
          if(result != 0)
            {
              break;
            }

          result = IFF_PeekFormHeader(reader, header);
        }
    }

  return block_file_api_globals.last_error;
}

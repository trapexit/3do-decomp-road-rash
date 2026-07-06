#include "aiff_sample_runtime.h"

int
IFF_ReadAIFFChunk(ByteFileReader *reader,
                  unsigned int    chunk_id,
                  int             start_position,
                  void           *destination,
                  int            *chunk_allocation_bytes,
                  unsigned int   *out_form_type)
{
  IffFormHeader form_header;
  int result;

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
          return 0;
        }
      if(out_form_type != 0)
        {
          *out_form_type = AIFC_FORM_ID;
        }
    }

  if(IFF_EnterForm(reader) != 0)
    {
      return 0;
    }

  return IFF_ReadChunk(reader, chunk_id, -1, (int)destination,
                       chunk_allocation_bytes);
}

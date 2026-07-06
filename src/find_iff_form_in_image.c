#include "string.h"

#include "iff_runtime.h"

void *
find_iff_form_in_image(const void  *form_data,
                       unsigned int form_type,
                       int         *out_form_size)
{
  const unsigned char *cursor;
  const unsigned char *image_end;
  void *matching_form;
  IffFormHeader form_header;

  matching_form = 0;
  if(out_form_size != 0)
    {
      *out_form_size = 0;
    }

  memcpy(&form_header, form_data, IFF_FORM_HEADER_SIZE);
  if(form_header.chunk_id == IFF_FORM_CHUNK_ID)
    {
      image_end = (const unsigned char *)form_data +
                  form_header.chunk_size + IFF_CHUNK_HEADER_SIZE;
      cursor = (const unsigned char *)form_data + IFF_FORM_HEADER_SIZE;
      while(cursor < image_end)
        {
          memcpy(&form_header, cursor, IFF_FORM_HEADER_SIZE);
          if(form_header.chunk_id == IFF_FORM_CHUNK_ID &&
             form_header.form_type == form_type)
            {
              matching_form = (void *)cursor;
              if(out_form_size != 0)
                {
                  *out_form_size = form_header.chunk_size;
                }
              break;
            }
          cursor += form_header.chunk_size;
        }
    }

  return matching_form;
}

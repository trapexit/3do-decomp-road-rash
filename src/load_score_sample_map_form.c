#include "aiff_sample_runtime.h"
#include "score_program_runtime.h"

int
DLL_AddTail(DLLList *list,
            int      value);
int
unload_sample_image(int sample_item, int release_sample_data);

int
load_score_sample_map_form(struct MidiPlayer *player,
                           ByteFileReader    *reader,
                           IffFormHeader     *sample_map_form,
                           DLLList           *sample_items)
{
  IffFormHeader form_header;
  int limit;
  int offset;
  int result;
  int size;
  int sample_item;

  if(sample_items == 0)
    {
      return -1;
    }

  result = enter_score_iff_form_payload(reader);
  if(result < 0)
    {
      return result;
    }

  size = sample_map_form->chunk_size;
  limit = (size + (size & IFF_CHUNK_ALIGNMENT_MASK)) -
          IFF_FORM_TYPE_SIZE;
  offset = 0;
  do
    {
      result = IFF_PeekFormHeader(
        reader, &form_header);
      if(result != 0)
        {
          return result;
        }

      result = load_aiff_sample_from_byte_file(
        reader, -1);
      if(result < 0)
        {
          return result;
        }
      sample_item = result;

      size = form_header.chunk_size;
      offset += size + (size & IFF_CHUNK_ALIGNMENT_MASK) +
                IFF_CHUNK_HEADER_SIZE;
      result = DLL_AddTail(sample_items, result);
      if(result != 0)
        {
          unload_sample_image(sample_item, 1);
          return result;
        }
    } while(offset < limit);

  return result;
}

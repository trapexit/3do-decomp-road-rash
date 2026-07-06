/* Reconstructed from the original function at 0x00030B44. */



#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

int
get_stream_input_buttons(unsigned int pad_number)
{
  int index;

  if(pad_number > STREAM_INPUT_PAD_COUNT || pad_number < 1)
    {
      return 0;
    }
  index = pad_number - 1;
  return (int)(stream_input_snapshots[index] & stream_input_masks[index]);
}

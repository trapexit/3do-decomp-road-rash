#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

int
get_raw_stream_input_buttons(unsigned int pad_number)
{
  if(pad_number > STREAM_INPUT_PAD_COUNT || pad_number < 1)
    {
      return 0;
    }

  return (int)stream_input_snapshots[pad_number - 1];
}

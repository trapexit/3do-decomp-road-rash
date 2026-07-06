
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

void
sample_stream_input_pads(void)
{
  int index;
  int value;

  index = 0;
  do
    {
      value = index + 1;
      stream_input_snapshots[index] =
        (unsigned int)read_control_pad_buttons(value);
      index = value;
    } while(value < STREAM_INPUT_PAD_COUNT);
}


void
set_stream_input_mask(unsigned int pad_number,
                      int          button_mask)
{
  if(pad_number <= STREAM_INPUT_PAD_COUNT && pad_number >= 1)
    {
      stream_input_masks[pad_number - 1] = (unsigned int)button_mask;
    }
}


int
get_stream_input_mask(unsigned int pad_number)
{
  if(pad_number > STREAM_INPUT_PAD_COUNT || pad_number < 1)
    {
      return -1;
    }
  return (int)stream_input_masks[pad_number - 1];
}

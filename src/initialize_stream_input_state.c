#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

void
initialize_stream_input_state(void)
{
  int index;

  for(index = 0; index < STREAM_INPUT_PAD_COUNT; index++)
    {
      stream_input_snapshots[index] = 0;
      stream_input_masks[index] = STREAM_INPUT_ALL_BUTTONS_MASK;
    }
}

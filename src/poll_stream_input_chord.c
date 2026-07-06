#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

int
poll_stream_input_chord(InputChordState *chord)
{
  int value;

  value = get_stream_input_buttons((int)chord->control_pad);
  return update_stream_input_chord_state(chord, (unsigned int)value);
}


int
poll_raw_stream_input_chord(InputChordState *chord)
{
  int value;

  value = get_raw_stream_input_buttons((unsigned int)chord->control_pad);
  return update_stream_input_chord_state(chord, (unsigned int)value);
}

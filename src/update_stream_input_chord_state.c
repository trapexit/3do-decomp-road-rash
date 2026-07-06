#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

int
update_stream_input_chord_state(InputChordState *chord,
                                unsigned int     input_buttons)
{
  unsigned int button_mask;
  int state;

  button_mask = chord->button_mask;
  state = chord->transition_state;
  if((input_buttons & button_mask) == button_mask)
    {
      if(state != STREAM_INPUT_CHORD_IDLE)
        {
          state = STREAM_INPUT_CHORD_HELD;
        }
      else
        {
          state = STREAM_INPUT_CHORD_JUST_PRESSED;
        }
    }
  else
    {
      if(state != STREAM_INPUT_CHORD_HELD)
        {
          state = STREAM_INPUT_CHORD_IDLE;
        }
      else
        {
          state = STREAM_INPUT_CHORD_JUST_RELEASED;
        }
    }

  chord->transition_state = state;
  return chord->transition_state;
}

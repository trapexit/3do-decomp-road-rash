#ifndef STREAM_INPUT_RUNTIME_H
#define STREAM_INPUT_RUNTIME_H

#include "rw_semantic_data.h"

#define STREAM_INPUT_PAD_COUNT 2
#define STREAM_INPUT_PAD_NUMBER_MASK 0xFF
#define STREAM_INPUT_ALL_BUTTONS_MASK (~0U)

enum StreamInputChordState
{
  STREAM_INPUT_CHORD_JUST_PRESSED = 0,
  STREAM_INPUT_CHORD_JUST_RELEASED = 1,
  STREAM_INPUT_CHORD_IDLE = 2,
  STREAM_INPUT_CHORD_HELD = 3
};

int
read_control_pad_buttons(int pad_number);
void
initialize_stream_input_state(void);
void
sample_stream_input_pads(void);
void
set_stream_input_mask(unsigned int pad_number,
                      int          button_mask);
int
get_stream_input_mask(unsigned int pad_number);
int
get_stream_input_buttons(unsigned int pad_number);
int
get_raw_stream_input_buttons(unsigned int pad_number);
int
update_stream_input_chord_state(InputChordState *chord,
                                unsigned int     input_buttons);
int
poll_stream_input_chord(InputChordState *chord);
int
poll_raw_stream_input_chord(InputChordState *chord);

#endif

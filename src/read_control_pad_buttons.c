#include "event.h"
#include "stream_input_runtime.h"
/* Reconstructed from the original function at 0x000294B0. */


int
read_control_pad_buttons(int pad_number)
{
  ControlPadEventData event;

  event.cped_ButtonBits = 0;
  GetControlPad(pad_number & STREAM_INPUT_PAD_NUMBER_MASK, 0, &event);
  return event.cped_ButtonBits;
}

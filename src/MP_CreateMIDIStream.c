/* Public tail-call wrapper around the stream constructor. */

#include "midi_player_runtime.h"

MidiStream *
MP_CreateMIDIStream(unsigned char *image,
                    int            image_size)
{
  return OMP_CreateMIDIStream(image, image_size);
}

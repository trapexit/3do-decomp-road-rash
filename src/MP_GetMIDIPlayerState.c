/* Reconstructed from the original function at 0x00040058. */

#include "midi_player_runtime.h"

int
MP_GetMIDIPlayerState(MidiPlayer *player)
{
  MidiPlayerRuntime *runtime;

  if(player != 0)
    {
      runtime = player->runtime;
      if(runtime != 0)
        {
          return runtime->state;
        }
    }
  return MIDI_ERROR_BAD_POINTER;
}

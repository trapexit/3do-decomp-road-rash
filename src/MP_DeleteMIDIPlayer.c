#include "midi_player_runtime.h"

int
OMP_DeleteMIDIPlayer(MidiPlayerRuntime *runtime);

int
MP_DeleteMIDIPlayer(MidiPlayer *player)
{
  int result;

  if(player != 0)
    {
      if(player->runtime != 0 &&
         player->runtime->state != MIDI_PLAYER_STATE_TERMINATED)
        {
          result = MP_StopMIDIPlayer(player);
          if(result < 0)
            {
              return result;
            }
        }
      result = OMP_DeleteMIDIPlayer(player->runtime);
      if(result < 0)
        {
          return result;
        }
      player->runtime = 0;
      delete_midi_player(player);
    }
  return 0;
}

#include "midi_player_runtime.h"
#include "score_context_api.h"

int
free_midi_player_channel_instruments(MidiPlayer *player)
{
  int index;
  int result;

  index = 0;
  do
    {
      result = FreeChannelInstruments(
        (ScoreContext *)player->score_context, index);
      if(result < 0)
        {
          return result;
        }
      index++;
    } while(index < MIDI_SCORE_PROGRAM_COUNT);

  return result;
}

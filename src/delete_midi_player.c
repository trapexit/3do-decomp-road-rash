#include "midi_player_runtime.h"
#include "runtime_data_types.h"
#include "score_context_api.h"


void
release_midi_player_resources(MidiPlayer *player)
{
  int result;

  if(player == 0)
    {
      return;
    }

  if(player->audio_clock_item >= 0)
    {
      release_midi_player_audio_clock(player);
    }

  unload_midi_player_score(player);

  if(player->score_context != 0)
    {
      result = DeleteScoreContext((ScoreContext *)player->score_context);
      if(result < 0)
        {
          music_library_globals.last_error = result;
        }
      player->score_context = 0;
    }

}

void
delete_midi_player(MidiPlayer *player)
{
  if(player == 0)
    {
      return;
    }
  release_midi_player_resources(player);
  music_library_globals.release((int)player, MIDI_PLAYER_ALLOCATION_SIZE);
}

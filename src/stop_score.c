#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
stop_score(void)
{
  MidiPlayer *player;

  if(game_audio_state.score_enabled != 0)
    {
      player = game_audio_state.score.player;
      if(player != 0)
        {
          return MP_StopMIDIPlayer(player);
        }
    }
  return 0;
}


int
pause_score(void)
{
  MidiPlayer *player;

  if(game_audio_state.score_enabled != 0)
    {
      player = game_audio_state.score.player;
      if(player != 0)
        {
          return MP_PauseMIDIPlayer(player);
        }
    }
  return 0;
}


int
resume_score(void)
{
  MidiPlayer *player;

  if(game_audio_state.score_enabled != 0)
    {
      player = game_audio_state.score.player;
      if(player != 0)
        {
          return MP_ResumeMIDIPlayer(player);
        }
    }
  return 0;
}

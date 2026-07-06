#include "operror.h"
#include "stdio.h"

#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
start_score(void)
{
  ScorePlaybackState *score;
  int result;

  result = 0;
  score = &game_audio_state.score;

  if(game_audio_state.score_enabled != 0 &&
     score->player != 0 && score->stream != 0)
    {
      result = MP_SetMIDIStream(score->player, score->stream,
                                MIDI_REPEAT_FOREVER);
      if(result == 0)
        {
          result = MP_StartMIDIPlayer(score->player,
                                      MIDI_REPEAT_FOREVER);
          if(result < 0)
            {
              printf("startScore:MP_StartMIDIPlayer\n");
              PrintfSysErr(result);
            }
        }
      else if(result < 0)
        {
          printf("startScore:MP_SetMIDIStream\n");
          PrintfSysErr(result);
        }
    }

  return result;
}

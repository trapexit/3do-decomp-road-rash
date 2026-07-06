#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
release_score_event_image(void *memory,
                          int   size);
void
shutdown_music_playback(void);

int
unload_score(void)
{
  ScorePlaybackState *score;
  int result;

  score = &game_audio_state.score;
  if(score->player != 0)
    {
      result = MP_DeleteMIDIPlayer(score->player);
      if(result < 0)
        {
          return result;
        }
      score->player = 0;
    }

  if(score->stream != 0)
    {
      MP_DeleteMIDIStream(score->stream);
      score->stream = 0;
    }

  if(score->event_image != 0)
    {
      release_score_event_image(score->event_image,
                                score->event_image_size);
      score->event_image = 0;
      score->event_image_size = 0;
    }

  shutdown_music_playback();
  return 0;
}

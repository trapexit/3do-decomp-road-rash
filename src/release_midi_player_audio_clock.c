#include "audio.h"
#include "midi_player_runtime.h"

int
release_midi_player_audio_clock(MidiPlayer *player)
{
  int item;
  int rate;
  int result;
  int disown_result;

  item = player->audio_clock_item;
  if(item < 0)
    {
      return item;
    }

  rate = player->saved_audio_rate;
  result = 0;
  if(rate > 0)
    {
      result = SetAudioRate(item, rate);
    }

  disown_result = DisownAudioClock(item);
  if(disown_result < 0)
    {
      if(result < 0)
        {
          return result;
        }
      return disown_result;
    }

  player->audio_clock_item = -1;
  player->saved_audio_rate = 0;
  return result;
}

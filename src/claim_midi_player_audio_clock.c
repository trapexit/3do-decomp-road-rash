#include "audio.h"
#include "midi_player_runtime.h"

int
claim_midi_player_audio_clock(MidiPlayer *player)
{
  int item;
  int rate;
  int result;
  int disown_result;

  item = OwnAudioClock();
  if(item < 0)
    {
      return item;
    }

  rate = GetAudioRate();
  result = SetAudioRate(item, MIDI_PLAYER_AUDIO_CLOCK_RATE);
  if(result < 0)
    {
      disown_result = DisownAudioClock(item);
      if(disown_result < 0)
        {
          player->audio_clock_item = item;
          player->saved_audio_rate = rate;
        }
      return result;
    }

  player->audio_clock_item = item;
  player->saved_audio_rate = rate;
  return item;
}

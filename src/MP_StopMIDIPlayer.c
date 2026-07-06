#include "audio.h"
#include "item.h"
#include "msgport.h"
/* Reconstructed from the original function at 0x0003FE9C. */

#include "midi_player_runtime.h"

int
MP_GetMIDIPlayerState(MidiPlayer *player);
int
MP_ResumeMIDIPlayer(MidiPlayer *player);
int
AM_WaitReply(int  message_port,
             int  reply_item,
             int *out_value,
             int *out_data,
             int *out_size);



int
MP_StopMIDIPlayer(MidiPlayer *player)
{
  int result;
  int timer_item;
  int player_state;

  result = 0;
  if(MP_GetMIDIPlayerState(player) == MIDI_PLAYER_STATE_PAUSED)
    {
      result = MP_ResumeMIDIPlayer(player);
      if(result < 0)
        {
          return result;
        }
    }

  if(MP_GetMIDIPlayerState(player) == MIDI_PLAYER_STATE_STOPPED ||
     MP_GetMIDIPlayerState(player) == MIDI_PLAYER_STATE_TERMINATED)
    {
      return result;
    }

  player->runtime->stop_requested = 1;
  timer_item = CreateSizedItem(MIDI_PLAYER_TIMER_ITEM_NODE, 0, 0);
  if(timer_item < 0)
    {
      player->runtime->stop_requested = 0;
      return timer_item;
    }

  player_state = MP_GetMIDIPlayerState(player);
  while(player_state != MIDI_PLAYER_STATE_STOPPED &&
        player_state != MIDI_PLAYER_STATE_TERMINATED)
    {
      result = GetAudioTime();
      result = SleepUntilTime(timer_item, result + 1);
      if(result < 0)
        {
          break;
        }
      player_state = MP_GetMIDIPlayerState(player);
    }

  DeleteItem(timer_item);
  player->runtime->stop_requested = 0;
  return result;
}


int
MP_PauseMIDIPlayer(MidiPlayer *player)
{
  int result;
  int reply_value;

  result = 0;
  reply_value = 0;
  if(MP_GetMIDIPlayerState(player) == MIDI_PLAYER_STATE_PLAYING)
    {
      result = SendSmallMsg(player->runtime->control_port,
                            player->runtime->reply_port,
                            MIDI_PLAYER_COMMAND_PAUSE, 0);
      if(result >= 0)
        {
          result = AM_WaitReply(player->runtime->creator_port,
                                player->runtime->reply_port,
                                &reply_value, 0, 0);
        }
    }
  return result;
}

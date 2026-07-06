#include "msgport.h"
/* Reconstructed from the original function at 0x0003FDF8. */

#include "midi_player_runtime.h"

int
MP_GetMIDIPlayerState(MidiPlayer *player);
int
AM_WaitReply(int  message_port,
             int  reply_item,
             int *out_value,
             int *out_data,
             int *out_size);

int
MP_StartMIDIPlayer(MidiPlayer *player,
                   int         repeat_count)
{
  int reply_value;
  int result;

  reply_value = 0;
  if(MP_GetMIDIPlayerState(player) != MIDI_PLAYER_STATE_STOPPED)
    {
      SendSmallMsg(player->runtime->control_port,
                   player->runtime->reply_port,
                   MIDI_PLAYER_COMMAND_RESET, 0);
      while(player->runtime->state != MIDI_PLAYER_STATE_STOPPED)
        {
        }
    }

  player->repeat_count = repeat_count;
  result = SendSmallMsg(player->runtime->control_port,
                        player->runtime->reply_port,
                        MIDI_PLAYER_COMMAND_START, 0);
  if(result < 0)
    {
      return result;
    }

  return AM_WaitReply(player->runtime->creator_port,
                      player->runtime->reply_port, &reply_value, 0, 0);
}

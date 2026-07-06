#include "midi_player_runtime.h"
#include "msgport.h"

int
MP_GetMIDIPlayerState(MidiPlayer *player);
int
AM_WaitReply(int  message_port,
             int  reply_item,
             int *out_value,
             int *out_data,
             int *out_size);

int
MP_ResumeMIDIPlayer(MidiPlayer *player)
{
  int result;
  int reply_value;

  result = 0;
  reply_value = 0;
  if(MP_GetMIDIPlayerState(player) != MIDI_PLAYER_STATE_PAUSED)
    {
      return result;
    }

  result = SendSmallMsg(player->runtime->control_port,
                        player->runtime->reply_port,
                        MIDI_PLAYER_COMMAND_RESUME, 0);
  if(result < 0)
    {
      return result;
    }

  return AM_WaitReply(player->runtime->creator_port,
                      player->runtime->reply_port, &reply_value, 0, 0);
}

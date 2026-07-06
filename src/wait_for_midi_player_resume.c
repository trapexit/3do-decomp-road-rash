#include "audio.h"
#include "audio_message_runtime.h"
#include "midi_player_runtime.h"
#include "msgport.h"

int
wait_for_midi_player_resume(MidiPlayer *player,
                            int        *time_offset)
{
  int32 message_type;
  Item message_item;
  int start;
  int result;
  int reply_result;
  int status;

  message_type = 0;
  message_item = 0;
  result = 0;
  reply_result = 0;

  player->runtime->state = MIDI_PLAYER_STATE_PAUSED;
  start = GetAudioTime();

  status = free_midi_player_channel_instruments(player);
  if(status >= 0)
    {
      status = AM_WaitMsg(player->runtime->control_port, &message_item,
                          &message_type, 0);
      if(status >= 0)
        {
          *time_offset += GetAudioTime() - start;
          if(message_type != MIDI_PLAYER_COMMAND_RESUME)
            {
              reply_result = MIDI_PLAYER_UNEXPECTED_RESUME_ERROR;
              result = MIDI_PLAYER_UNEXPECTED_RESUME_ERROR;
            }
          status = ReplySmallMsg(message_item, reply_result, 0, 0);
        }
    }

  player->runtime->state = MIDI_PLAYER_STATE_PLAYING;
  if(result < 0)
    {
      return result;
    }
  return status;
}

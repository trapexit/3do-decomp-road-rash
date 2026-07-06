#include "midi_player_runtime.h"

int
MP_StopMIDIPlayer(MidiPlayer *player);
int
MP_StartMIDIPlayer(MidiPlayer *player,
                   int         repeat_count);

int
MP_SetMIDIStream(MidiPlayer *player,
                 MidiStream *stream,
                 int         repeat_count)
{
  int result;
  int restart_player;

  result = 0;
  restart_player = 0;

  if(player->runtime->state != MIDI_PLAYER_STATE_STOPPED)
    {
      MP_StopMIDIPlayer(player);
      restart_player = 1;
    }

  player->stream = stream;
  stream->score_context = player->score_context;
  stream->remaining_repeats = repeat_count;

  if(restart_player != 0)
    {
      result = MP_StartMIDIPlayer(player, repeat_count);
    }

  return result;
}


int
stage_midi_stream(MidiPlayer *player,
                  MidiStream *stream,
                  int         repeat_count)
{
  if(player->runtime->state != MIDI_PLAYER_STATE_STOPPED)
    {
      player->pending_stream = stream;
    }
  else
    {
      player->stream = stream;
    }

  stream->score_context = player->score_context;
  stream->remaining_repeats = repeat_count;
  return 0;
}


int
MP_SetMIDIPlayerCallback(MidiPlayer        *player,
                         MidiPlayerCallback callback,
                         int                callback_context)
{
  player->callback = (int)callback;
  player->callback_context = callback_context;
  return 0;
}

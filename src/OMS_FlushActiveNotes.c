#include "midi_player_runtime.h"

int
OMS_ReleaseActiveNotes(MidiStream  *stream,
                       unsigned int current_time,
                       unsigned int flush_all,
                       int          callback_context);

int
OMS_FlushActiveNotes(MidiStream *stream)
{
  return OMS_ReleaseActiveNotes(stream, 0, 1, 0);
}

#include "byte_file_runtime.h"
#include "midi_player_runtime.h"

int
load_midi_player_score_file(MidiPlayer *player,
                            const char *path)
{
  int result;
  ByteFileReader *reader;

  if(path == 0)
    {
      return MIDI_ERROR_BAD_POINTER;
    }

  reader = BF_Open(path, 0);
  if(reader == 0)
    {
      return -1;
    }

  result = load_midi_player_score_stream(player, reader, 0, 0);
  if(reader != 0)
    {
      BF_Close(reader);
    }
  return result;
}

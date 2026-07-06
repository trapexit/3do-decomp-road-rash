#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
OMP_DeleteMIDIStream(MidiStream *stream)
{
  int (*release_stream)(int,
                        int);

  if(stream == 0)
    {
      return 0;
    }

  release_stream = (int (*)(int, int)) music_library_globals.release;
  if(stream->tracks != 0)
    {
      release_stream((int)stream->tracks,
                     stream->track_count * MIDI_TRACK_STATE_SIZE);
    }

  return release_stream((int)stream, MIDI_STREAM_ALLOCATION_SIZE);
}

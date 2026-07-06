/* Reconstructed from the original function at 0x0003F24C. */



#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
MP_DeleteMIDIStream(MidiStream *stream)
{
  return OMP_DeleteMIDIStream(stream);
}


MidiStream *
OMP_CreateMIDIStream(unsigned char *image,
                     int            image_size)
{
  MidiStream *stream;
  int parse_result;
  MidiStream *(*allocate_stream)(int size,
                                 int flags);

  stream = 0;
  if(image == 0 || image_size < IFF_FORM_HEADER_SIZE)
    {
      music_library_globals.last_error = MIDI_ERROR_BAD_POINTER;
      return 0;
    }

  allocate_stream =
    (MidiStream * (*)(int, int)) music_library_globals.allocate;
  stream = allocate_stream(MIDI_STREAM_ALLOCATION_SIZE, 0);
  if(stream == 0)
    {
      music_library_globals.last_error = MIDI_ERROR_NO_MEMORY;
      return 0;
    }

  stream->image = image;
  stream->score_context = 0;
  stream->start_time = 0;
  stream->image_size = image_size;
  stream->reserved_10 = 0;
  stream->track_count = 0;
  stream->tracks = 0;
  stream->remaining_repeats = 0;

  parse_result = OMP_ParseMIDIStreamImage(stream, image_size);
  music_library_globals.last_error = parse_result;
  if(parse_result < 0)
    {
      OMP_DeleteMIDIStream(stream);
      stream = 0;
    }

  return stream;
}

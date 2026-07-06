#include "midi_player_runtime.h"

int
OMS_DecodeVariableLength(unsigned char *cursor,
                         int           *out_bytes_consumed);

int
OMS_ResetMIDIStream(MidiStream  *stream,
                    unsigned int start_time)
{
  int decode_result[MIDI_DECODE_RESULT_WORDS];
  int track_index;
  MidiTrackState *track;
  int first_delta_time;

  decode_result[1] = 0;
  decode_result[0] = 0;
  stream->start_time = start_time;

  track_index = 0;
  while(track_index < stream->track_count)
    {
      track = &stream->tracks[track_index];
      track->cursor = track->data;
      first_delta_time =
        OMS_DecodeVariableLength(track->data, decode_result);
      track->next_event_time = start_time + first_delta_time;
      track->cursor = track->data + decode_result[0];
      track->active = 1;
      track_index++;
    }

  stream->remaining_repeats--;
  return decode_result[1];
}

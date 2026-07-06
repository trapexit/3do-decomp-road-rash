#include "audio.h"
#include "debug.h"
#include "score_context_api.h"
#include "stdio.h"

#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
OMS_ResetMIDIStream(MidiStream  *stream,
                    unsigned int start_time);
int
OMS_FlushActiveNotes(MidiStream *stream);
int
OMS_RegisterActiveNote(MidiStream     *stream,
                       MidiTrackState *track,
                       unsigned int    event_time,
                       int             channel,
                       int             note_number,
                       int             duration);
int
OMS_ReleaseActiveNotes(MidiStream  *stream,
                       unsigned int current_time,
                       unsigned int flush_all,
                       int          callback_context);
int
OMS_DecodeVariableLength(unsigned char *cursor,
                         int           *out_bytes_consumed);
int
OMS_BumpMIDIStream(MidiStream   *stream,
                   unsigned int  current_time,
                   unsigned int *out_next_event_time,
                   int          *out_time_offset)
{
  unsigned char status;
  unsigned char controller_number;
  unsigned char note_number;
  unsigned char midi_command;
  unsigned char *cursor;
  unsigned char *message_source;
  unsigned char *message_destination;
  unsigned char message[MIDI_MESSAGE_BUFFER_SIZE];
  MidiTrackState *track;
  int track_index;
  int active_note_index;
  int midi_data_byte_count;
  int decoded_byte_count;
  int result;
  unsigned int next_event_time;
  unsigned int candidate_time;
  int delta_time;
  int playback_time;
  int active_track_found;

  result = OMS_ReleaseActiveNotes(stream, current_time, 0,
                                  (int)out_time_offset);
  if(result < 0)
    {
      return result;
    }

  controller_number = MIDI_NO_CONTROLLER;
  note_number = 0;

  track_index = 0;
  while(track_index < stream->track_count)
    {
      track = &stream->tracks[track_index];
      if(track->active != 0)
        {
          cursor = track->cursor;
          if(track->next_event_time <= current_time)
            {
              status = *cursor;
              if((status & MIDI_STATUS_PRESENT) == 0)
                {
                  status = track->running_status;
                }
              else
                {
                  cursor++;
                  track->running_status = status;
                }

              if(status == MIDI_META_EVENT)
                {
                  track->active = 0;
                }
              else
                {
                  midi_command = status & MIDI_COMMAND_MASK;
                  if(midi_command == MIDI_CONTROL_CHANGE)
                    {
                      midi_data_byte_count = MIDI_TWO_DATA_BYTES;
                      controller_number = *cursor;
                    }
                  else if(midi_command == MIDI_NOTE_ON)
                    {
                      midi_data_byte_count = MIDI_TWO_DATA_BYTES;
                      note_number = *cursor;
                    }
                  else if(midi_command == MIDI_NOTE_OFF ||
                          midi_command == MIDI_POLYPHONIC_AFTERTOUCH ||
                          midi_command == MIDI_PITCH_BEND)
                    {
                      midi_data_byte_count = MIDI_TWO_DATA_BYTES;
                    }
                  else if(midi_command == MIDI_PROGRAM_CHANGE ||
                          midi_command == MIDI_CHANNEL_PRESSURE)
                    {
                      midi_data_byte_count = MIDI_ONE_DATA_BYTE;
                    }
                  else
                    {
                      kprintf(
                        "ERROR: OMS_BumpMIDIStream, Illegal MIDI Message Status! %X\n",
                        status);
                      fflush(stdout);
                      return MIDI_ERROR_ILLEGAL_MESSAGE_STATUS;
                    }

                  message[0] = status;
                  message_source = cursor;
                  message_destination = message + 1;
                  while(midi_data_byte_count > 0)
                    {
                      *message_destination++ = *message_source++;
                      midi_data_byte_count--;
                    }
                  cursor = message_source;

                  if(midi_command == MIDI_NOTE_ON)
                    {
                      delta_time = OMS_DecodeVariableLength(
                        cursor, &decoded_byte_count);
                      cursor += decoded_byte_count;
                      if(OMS_RegisterActiveNote(
                           stream, track, current_time,
                           status & MIDI_CHANNEL_MASK, note_number,
                           delta_time) == 0)
                        {
                          result = InterpretMIDIMessage(
                            (ScoreContext *)stream->score_context,
                            (char *)message, 0);
                          if(result < 0)
                            {
                              return result;
                            }
                        }
                    }
                  else
                    {
                      result = InterpretMIDIMessage(
                        (ScoreContext *)stream->score_context,
                        (char *)message, 0);
                      if(result < 0)
                        {
                          return result;
                        }
                    }

                  delta_time = OMS_DecodeVariableLength(
                    cursor, &decoded_byte_count);
                  track->cursor = cursor + decoded_byte_count;
                  track->next_event_time = current_time + delta_time;
                }
            }
        }
      track_index++;
    }

  active_track_found = 0;
  next_event_time = MIDI_NO_EVENT_TIME;
  // Inspect deadlines after dispatch so ended tracks and newly registered notes are final.
  track_index = 0;
  while(track_index < stream->track_count)
    {
      track = &stream->tracks[track_index];
      if(track->active != 0)
        {
          active_track_found = 1;
          candidate_time = track->next_event_time;
          if(candidate_time < next_event_time)
            {
              next_event_time = candidate_time;
            }

          for(active_note_index = 0;
              active_note_index < track->active_note_count;
              active_note_index++)
            {
              candidate_time = track->active_notes[active_note_index].release_time;
              if(candidate_time < next_event_time)
                next_event_time = candidate_time;
            }
        }

      track_index++;
    }

  if(active_track_found != 0)
    {
      *out_next_event_time = next_event_time;
      if(controller_number == MIDI_ALL_SOUND_OFF_CONTROLLER)
        {
          return MIDI_ALL_SOUND_OFF_CONTROLLER;
        }
      return 0;
    }

  if(stream->remaining_repeats < 1)
    {
      return 1;
    }

  result = OMS_FlushActiveNotes(stream);
  if(result < 0)
    {
      return result;
    }

  playback_time = GetAudioTime();
  result = OMS_ResetMIDIStream(stream, (unsigned int)playback_time);
  if(result < 0)
    {
      return result;
    }

  stream->remaining_repeats--;
  *out_time_offset = 0;
  *out_next_event_time = (unsigned int)playback_time;
  if(controller_number == MIDI_ALL_SOUND_OFF_CONTROLLER)
    {
      return MIDI_ALL_SOUND_OFF_CONTROLLER;
    }
  return 0;
}

#include "midi_player_runtime.h"
#include "score_context_api.h"

int
OMS_RegisterActiveNote(MidiStream     *stream,
                       MidiTrackState *track,
                       unsigned int    event_time,
                       int             channel,
                       int             note_number,
                       int             duration)
{
  MidiActiveNoteState *active_note;
  int active_note_index;
  unsigned int note_off_message;
  int result;
  unsigned int release_time;
  unsigned char note_off_status;
  unsigned char midi_note;

  note_off_status = (unsigned char)(channel | MIDI_NOTE_OFF);
  midi_note = (unsigned char)note_number;
  release_time = event_time + duration;
  result = 0;
  active_note_index = 0;
  if(track->active_note_count > 0)
    {
      do
        {
          active_note = &track->active_notes[active_note_index];
          if(active_note->note_number == midi_note)
            {
              note_off_message =
                ((unsigned int)active_note->note_off_status <<
                  MIDI_MESSAGE_STATUS_SHIFT) |
                ((unsigned int)active_note->note_number <<
                  MIDI_MESSAGE_DATA_SHIFT);
              result = InterpretMIDIMessage(
                (ScoreContext *)stream->score_context,
                (char *)&note_off_message, 0);
              if(result >= 0)
                {
                  active_note->release_time = release_time;
                }
              return result;
            }
          active_note_index++;
        } while(active_note_index < track->active_note_count);
    }

  if(track->active_note_count >= MIDI_ACTIVE_NOTES_PER_TRACK)
    {
      return 1;
    }

  active_note = &track->active_notes[track->active_note_count];
  active_note->note_off_status = note_off_status;
  active_note->note_number = midi_note;
  active_note->release_time = release_time;
  track->active_note_count++;
  return result;
}

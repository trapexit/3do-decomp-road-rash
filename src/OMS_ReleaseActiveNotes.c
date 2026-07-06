#include "midi_player_runtime.h"
#include "score_context_api.h"

int
OMS_ReleaseActiveNotes(MidiStream  *stream,
                       unsigned int current_time,
                       unsigned int flush_all,
                       int          callback_context)
{
  int result;
  int track_index;
  int active_note_index;
  MidiTrackState *track;
  MidiActiveNoteState *active_note;
  unsigned int note_off_message;
  int last_active_note_index;

  result = 0;
  track_index = 0;
  (void)callback_context;

  while(track_index < stream->track_count)
    {
      track = &stream->tracks[track_index];
      if(track->active != 0)
        {
          active_note_index = 0;
          while(active_note_index < track->active_note_count)
            {
              active_note = &track->active_notes[active_note_index];
              if(active_note->release_time <= current_time ||
                 (flush_all & MIDI_DATA_MASK) != 0)
                {
                  note_off_message =
                    ((unsigned int)active_note->note_off_status <<
                      MIDI_MESSAGE_STATUS_SHIFT) |
                    ((unsigned int)active_note->note_number <<
                      MIDI_MESSAGE_DATA_SHIFT);
                  result = InterpretMIDIMessage(
                    (ScoreContext *)stream->score_context,
                    (char *)&note_off_message, 0);
                  if(result < 0)
                    {
                      return result;
                    }

                  last_active_note_index = track->active_note_count - 1;
                  track->active_note_count = last_active_note_index;
                  /*
                   * The retail routine has already advanced active_note to
                   * active_notes[active_note_index], then indexes from that
                   * pointer again for both operands.
                   */
                  active_note[active_note_index] =
                    active_note[last_active_note_index];
                }
              active_note_index++;
            }
        }
      track_index++;
    }

  return result;
}

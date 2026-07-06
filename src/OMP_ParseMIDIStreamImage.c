#include "limits.h"
#include "string.h"

#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

enum
{
  SCORE_WORD_FIRST_BYTE_SHIFT = 24,
  SCORE_WORD_SECOND_BYTE_SHIFT = 16,
  SCORE_WORD_THIRD_BYTE_SHIFT = 8,
  SCORE_WORD_FIRST_BYTE_INDEX = 0,
  SCORE_WORD_SECOND_BYTE_INDEX = 1,
  SCORE_WORD_THIRD_BYTE_INDEX = 2,
  SCORE_WORD_FOURTH_BYTE_INDEX = 3
};

static unsigned int
read_score_word(const unsigned char *data)
{
  return ((unsigned int)data[SCORE_WORD_FIRST_BYTE_INDEX] <<
            SCORE_WORD_FIRST_BYTE_SHIFT) |
         ((unsigned int)data[SCORE_WORD_SECOND_BYTE_INDEX] <<
            SCORE_WORD_SECOND_BYTE_SHIFT) |
         ((unsigned int)data[SCORE_WORD_THIRD_BYTE_INDEX] <<
            SCORE_WORD_THIRD_BYTE_SHIFT) |
         (unsigned int)data[SCORE_WORD_FOURTH_BYTE_INDEX];
}

static int
read_iff_chunk(const unsigned char *image,
               int                  image_size,
               int                  chunk_offset,
               IffChunkHeader      *out_header,
               int                 *out_next_offset)
{
  int padded_size;
  int remaining;

  if(chunk_offset < 0 || chunk_offset > image_size ||
     image_size - chunk_offset < IFF_CHUNK_HEADER_SIZE)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }

  out_header->chunk_id = read_score_word(image + chunk_offset);
  out_header->chunk_size =
    (int)read_score_word(image + chunk_offset + sizeof(unsigned int));
  if(out_header->chunk_size < 0 || out_header->chunk_size == INT_MAX)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }

  padded_size = out_header->chunk_size +
                (out_header->chunk_size & IFF_CHUNK_ALIGNMENT_MASK);
  remaining = image_size - chunk_offset - IFF_CHUNK_HEADER_SIZE;
  if(padded_size > remaining)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }

  *out_next_offset = chunk_offset + IFF_CHUNK_HEADER_SIZE + padded_size;
  return 0;
}

static int
inspect_track_form(const unsigned char *image,
                   int                  form_offset,
                   int                  form_end,
                   int                 *out_track_data_offset)
{
  IffChunkHeader chunk_header;
  int chunk_offset;
  int next_offset;
  int result;

  *out_track_data_offset = -1;
  chunk_offset = form_offset + IFF_FORM_HEADER_SIZE;
  while(chunk_offset < form_end)
    {
      result = read_iff_chunk(image, form_end, chunk_offset,
                              &chunk_header, &next_offset);
      if(result < 0)
        {
          return result;
        }
      if(chunk_header.chunk_id == EAMS_TRACK_DATA_CHUNK_ID)
        {
          if(*out_track_data_offset >= 0)
            {
              return MIDI_ERROR_BAD_SCORE_IMAGE;
            }
          *out_track_data_offset = chunk_offset + IFF_CHUNK_HEADER_SIZE;
        }
      chunk_offset = next_offset;
    }

  if(chunk_offset != form_end || *out_track_data_offset < 0)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }
  return 0;
}

static void
initialize_track_state(MidiTrackState *track_state,
                       unsigned char  *track_data)
{
  int active_note_index;

  memset(track_state, 0, MIDI_TRACK_STATE_SIZE);
  track_state->data = track_data;
  track_state->cursor = track_data;
  active_note_index = 0;
  while(active_note_index < MIDI_ACTIVE_NOTES_PER_TRACK)
    {
      track_state->active_notes[active_note_index].note_off_status = 0;
      track_state->active_notes[active_note_index].note_number = 0;
      track_state->active_notes[active_note_index].release_time = 0;
      active_note_index++;
    }
}

int
OMP_ParseMIDIStreamImage(MidiStream *stream,
                         int         image_size)
{
  IffChunkHeader chunk_header;
  unsigned char *image;
  MidiTrackState *tracks;
  MidiTrackState *(*allocate_tracks)(int size,
                                     int flags);
  int root_end;
  int root_size;
  int chunk_offset;
  int next_offset;
  int track_form_end;
  int track_data_offset;
  int declared_track_count;
  int found_track_count;
  int track_index;
  int allocation_size;
  int found_global_info;
  int result;

  if(stream == 0 || stream->image == 0 ||
     image_size < IFF_FORM_HEADER_SIZE)
    {
      return MIDI_ERROR_BAD_POINTER;
    }

  image = stream->image;
  if(read_score_word(image) != IFF_FORM_CHUNK_ID)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }
  root_size = (int)read_score_word(image + sizeof(unsigned int));
  if(root_size < IFF_FORM_TYPE_SIZE ||
     root_size > image_size - IFF_CHUNK_HEADER_SIZE)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }
  root_end = root_size + IFF_CHUNK_HEADER_SIZE;

  found_global_info = 0;
  found_track_count = 0;
  declared_track_count = 0;
  chunk_offset = IFF_FORM_HEADER_SIZE;
  while(chunk_offset < root_end)
    {
      result = read_iff_chunk(image, root_end, chunk_offset,
                              &chunk_header, &next_offset);
      if(result < 0)
        {
          return result;
        }

      if(chunk_header.chunk_id == EAMS_GLOBAL_INFO_CHUNK_ID)
        {
          if(found_global_info ||
             chunk_header.chunk_size <
               EAMS_GLOBAL_INFO_WORDS * (int)sizeof(unsigned int))
            {
              return MIDI_ERROR_BAD_SCORE_IMAGE;
            }
          declared_track_count =
            (int)read_score_word(image + chunk_offset + IFF_CHUNK_HEADER_SIZE +
                                 EAMS_TRACK_COUNT_INDEX *
                                   (int)sizeof(unsigned int)) &
            EAMS_TRACK_COUNT_MASK;
          found_global_info = 1;
        }
      else if(chunk_header.chunk_id == IFF_FORM_CHUNK_ID)
        {
          if(chunk_header.chunk_size < IFF_FORM_TYPE_SIZE)
            {
              return MIDI_ERROR_BAD_SCORE_IMAGE;
            }
          if(read_score_word(image + chunk_offset + IFF_CHUNK_HEADER_SIZE) ==
             EAMS_TRACK_FORM_ID)
            {
              track_form_end = chunk_offset + IFF_CHUNK_HEADER_SIZE +
                               chunk_header.chunk_size;
              result = inspect_track_form(image, chunk_offset, track_form_end,
                                          &track_data_offset);
              if(result < 0)
                {
                  return result;
                }
              if(found_track_count == INT_MAX)
                {
                  return MIDI_ERROR_BAD_SCORE_IMAGE;
                }
              found_track_count++;
            }
        }
      chunk_offset = next_offset;
    }

  if(chunk_offset != root_end || !found_global_info)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }
  if(declared_track_count != found_track_count ||
     declared_track_count > INT_MAX / MIDI_TRACK_STATE_SIZE)
    {
      return MIDI_ERROR_BAD_SCORE_IMAGE;
    }

  tracks = 0;
  allocation_size = declared_track_count * MIDI_TRACK_STATE_SIZE;
  if(allocation_size > 0)
    {
      allocate_tracks = (MidiTrackState * (*)(int, int))
                        music_library_globals.allocate;
      if(allocate_tracks == 0)
        {
          return MIDI_ERROR_NO_MEMORY;
        }
      tracks = allocate_tracks(allocation_size, 0);
      if(tracks == 0)
        {
          return MIDI_ERROR_NO_MEMORY;
        }
    }

  track_index = 0;
  chunk_offset = IFF_FORM_HEADER_SIZE;
  while(chunk_offset < root_end)
    {
      result = read_iff_chunk(image, root_end, chunk_offset,
                              &chunk_header, &next_offset);
      if(result < 0)
        {
          return result;
        }
      if(chunk_header.chunk_id == IFF_FORM_CHUNK_ID)
        {
          if(read_score_word(image + chunk_offset + IFF_CHUNK_HEADER_SIZE) ==
             EAMS_TRACK_FORM_ID)
            {
              track_form_end = chunk_offset + IFF_CHUNK_HEADER_SIZE +
                               chunk_header.chunk_size;
              result = inspect_track_form(image, chunk_offset, track_form_end,
                                          &track_data_offset);
              if(result < 0)
                {
                  return result;
                }
              initialize_track_state(&tracks[track_index],
                                     image + track_data_offset);
              track_index++;
            }
        }
      chunk_offset = next_offset;
    }

  stream->track_count = declared_track_count;
  stream->tracks = tracks;
  return 0;
}

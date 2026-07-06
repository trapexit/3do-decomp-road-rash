/* Reconstructed from the original function at 0x0003FC34. */



#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
find_music_tag_value(MusicTagValue *tags,
                     int            tag_id,
                     int           *out_value,
                     int           *out_tag_index);
MidiPlayer *
OMP_CreateMIDIPlayer(MidiPlayerConfig *config);

MidiPlayer *
MP_CreateMIDIPlayer(MusicTagValue *tags)
{
  MidiPlayerConfig config;
  int *tag_data;

  config.score_image = 0;
  config.file_handle = 0;
  config.file_offset = 0;
  config.file_length = 0;
  config.thread_priority = 0;
  config.priority_padding[0] = 0;
  config.priority_padding[1] = 0;
  config.priority_padding[MIDI_PLAYER_PRIORITY_PADDING_LAST] = 0;
  config.runtime_state = 0;
  config.mixer_amplitude = 0;
  tag_data = 0;

  if(find_music_tag_value(tags, MP_TAG_SCORE_FILE,
                          (int *)&tag_data, 0) != 0)
    {
      config.file_handle = (struct ByteFileReader *)
                           tag_data[MIDI_PLAYER_FILE_HANDLE_INDEX];
      config.file_offset = tag_data[MIDI_PLAYER_FILE_OFFSET_INDEX];
      config.file_length = tag_data[MIDI_PLAYER_FILE_LENGTH_INDEX];
    }
  else
    {
      if(find_music_tag_value(tags, MP_TAG_SCORE_IMAGE,
                              (int *)&tag_data, 0) == 0)
        {
          music_library_globals.last_error = MIDI_ERROR_BAD_POINTER;
          return 0;
        }
      config.score_image = tag_data;
    }

  if(find_music_tag_value(tags, MP_TAG_THREAD_PRIORITY,
                          (int *)&tag_data, 0) == 0)
    {
      config.thread_priority = MIDI_PLAYER_DEFAULT_THREAD_PRIORITY;
    }
  else
    {
      config.thread_priority = (unsigned char)(int)tag_data;
    }

  if(find_music_tag_value(tags, MP_TAG_MIXER_AMPLITUDE,
                          (int *)&tag_data, 0) == 0)
    {
      config.mixer_amplitude = MIDI_PLAYER_DEFAULT_MIXER_AMPLITUDE;
    }
  else
    {
      config.mixer_amplitude = (int)tag_data;
    }

  return OMP_CreateMIDIPlayer(&config);
}

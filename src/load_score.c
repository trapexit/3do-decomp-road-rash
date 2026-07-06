#include "operror.h"
#include "stdio.h"
#include "string.h"

#include "byte_file_runtime.h"
#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

#define SCORE_ASSET_PATH_CAPACITY       256
#define SCORE_PLAYER_TAG_COUNT          4
#define SCORE_MIXER_TAG_INDEX           2
#define SCORE_END_TAG_INDEX             3
#define RACE_SCORE_MIXER_AMPLITUDE      0x7998

typedef struct MidiPlayerFileSource
{
  ByteFileReader *reader;
  int offset;
  int length;
} MidiPlayerFileSource;

void *
allocate_score_event_image(int size,
                           int flags);
int
initialize_music_playback(void);
int
unload_score(void);

static
const
char *
current_score_course_name(void)
{
  signed char player_index;
  signed char variant_index;

  player_index = race_mode_state.active_player_index;
  variant_index = player_profiles[player_index].current_variant;
  return course_variant_names[variant_index];
}


static
void
build_score_asset_path(char       *path,
                       const char *extension)
{
  const char *course_name;

  course_name = current_score_course_name();
  strcpy(path, resource_root_path);
  strcat(path, course_name);
  strcat(path, "/");
  strcat(path, course_name);
  strcat(path, extension);
}


int
load_score(void)
{
  char path[SCORE_ASSET_PATH_CAPACITY];
  MidiPlayerFileSource player_file;
  MusicTagValue player_tags[SCORE_PLAYER_TAG_COUNT];
  ByteFileReader *event_file;
  MidiPlayer *player;
  MidiStream *stream;
  void *event_image;
  int result;
  int setup_result;
  int event_image_size;

  result = unload_score();
  if(result < 0)
    {
      return result;
    }

  game_audio_state.score_enabled = race_mode_state.music_enabled;
  if(game_audio_state.score_enabled == 0)
    {
      return 0;
    }

  setup_result = initialize_music_playback();
  if(setup_result < 0)
    {
      game_audio_state.score_enabled = 0;
      return setup_result;
    }

  build_score_asset_path(path, game_audio_state.score_configuration_suffix);
  player_file.reader = BF_Open(path, 0);
  if(player_file.reader == 0)
    {
      printf("loadScore:BF_Open");
      PrintfSysErr(-1);
      game_audio_state.score_enabled = 0;
      unload_score();
      return -1;
    }

  player_file.offset = 0;
  player_file.length = 0;
  player_tags[0].tag = MP_TAG_SCORE_FILE;
  player_tags[0].value = (int)&player_file;
  player_tags[1].tag = MP_TAG_THREAD_PRIORITY;
  player_tags[1].value = MIDI_PLAYER_DEFAULT_THREAD_PRIORITY;
  player_tags[SCORE_MIXER_TAG_INDEX].tag = MP_TAG_MIXER_AMPLITUDE;
  player_tags[SCORE_MIXER_TAG_INDEX].value = RACE_SCORE_MIXER_AMPLITUDE;
  player_tags[SCORE_END_TAG_INDEX].tag = 0;

  player = MP_CreateMIDIPlayer(player_tags);
  if(player == 0)
    {
      result = score_loader_last_error;
      if(result == 0)
        {
          result = -1;
        }
      if(result < 0)
        {
          printf("loadScore:MP_CreateMIDIPlayer");
          PrintfSysErr(result);
        }
      BF_Close(player_file.reader);
      game_audio_state.score_enabled = 0;
      unload_score();
      return result;
    }

  BF_Close(player_file.reader);
  player_file.reader = 0;
  game_audio_state.score.player = player;

  build_score_asset_path(path, game_audio_state.score_event_suffix);
  event_file = BF_Open(path, 0);
  if(event_file == 0)
    {
      printf("DloadScore:BF_Open(fileName)");
      PrintfSysErr(-1);
      game_audio_state.score_enabled = 0;
      unload_score();
      return -1;
    }

  event_image_size = (int)event_file->file_status.fs_ByteCount;
  if(event_image_size < 0)
    {
      BF_Close(event_file);
      game_audio_state.score_enabled = 0;
      unload_score();
      return event_image_size;
    }

  event_image = allocate_score_event_image(event_image_size, 0);
  if(event_image == 0)
    {
      BF_Close(event_file);
      game_audio_state.score_enabled = 0;
      unload_score();
      return MIDI_ERROR_NO_MEMORY;
    }

  game_audio_state.score.event_image = event_image;
  game_audio_state.score.event_image_size = event_image_size;

  result = BF_Read(event_file, event_image, event_image_size);
  if(result != event_image_size)
    {
      printf("loadScore:BF_Read(_EAMS_DiskStream)\n");
      if(result < 0)
        {
          PrintfSysErr(result);
        }
      game_audio_state.score_enabled = 0;
      BF_Close(event_file);
      unload_score();
      return (result < 0) ? result : MIDI_ERROR_BAD_SCORE_IMAGE;
    }

  BF_Close(event_file);

  stream = MP_CreateMIDIStream((unsigned char *)event_image,
                               event_image_size);
  if(stream == 0)
    {
      result = music_library_globals.last_error;
      if(result == 0)
        {
          result = MIDI_ERROR_BAD_SCORE_IMAGE;
        }
      if(result < 0)
        {
          printf("loadScore:MP_CreateMIDIStream(eamsImage)\n");
          PrintfSysErr(result);
        }
      game_audio_state.score_enabled = 0;
      unload_score();
      return result;
    }

  game_audio_state.score.stream = stream;
  return 0;
}

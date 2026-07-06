#include "string.h"

#include "audio.h"
#include "item.h"

#include "async_stream_runtime.h"
#include "movie_playback_runtime.h"
#include "rw_semantic_data.h"
#include "stream_audio_playback.h"
#include "stream_input_runtime.h"

void
queue_fullscreen_clear(int screen_index);
void
present_display_frame(int screen_index);
void
handle_movie_frame_transition_cue(void *unused_context,
                                  int   relative_stream_time);
int
open_async_stream(AsyncStreamPlaybackStatePrefix **state,
                  const char                      *path,
                  int                              stream_unit_count,
                  const RwTagArgument             *tags);
int
start_async_stream_playback(AsyncStreamPlaybackStatePrefix *state,
                            const RwTagArgument            *tags);

enum MovieStreamTagIndex
{
  MOVIE_TAG_AUDIO_CHUNK_CALLBACK_INDEX,
  MOVIE_TAG_AUDIO_INITIALIZE_CALLBACK_INDEX,
  MOVIE_TAG_FIRST_SCREEN_INDEX,
  MOVIE_TAG_SECOND_SCREEN_INDEX,
  MOVIE_TAG_FRAME_X_INDEX,
  MOVIE_TAG_BUFFER_GROUP_INDEX,
  MOVIE_TAG_FRAME_Y_INDEX,
  MOVIE_TAG_TERMINATOR_INDEX,
  MOVIE_TAG_COUNT,
  MOVIE_PLAYBACK_TAG_WAIT_INDEX = MOVIE_TAG_AUDIO_CHUNK_CALLBACK_INDEX,
  MOVIE_PLAYBACK_TAG_TERMINATOR_INDEX =
    MOVIE_TAG_AUDIO_INITIALIZE_CALLBACK_INDEX
};

static AsyncStreamPlaybackStatePrefix *retained_movie_stream_state;

int
stop_retained_movie_stream(void)
{
  int stop_accepted;
  int result;

  result = stop_async_stream(retained_movie_stream_state, &stop_accepted);
  if(stop_accepted != 0)
    {
      retained_movie_stream_state = 0;
    }
  return result;
}

static
void
flip_async_movie_display_pair(void)
{
  queue_fullscreen_clear(0);
  present_display_frame(1);
  queue_fullscreen_clear(0);
  present_display_frame(1);
}


static int
build_movie_stream_path(char       *path,
                        unsigned int path_capacity,
                        const char *stream_name)
{
  unsigned int root_bytes;
  unsigned int directory_bytes;
  unsigned int name_bytes;

  if(path == 0 || stream_name == 0)
    {
      return 0;
    }
  root_bytes = strlen(resource_root_path);
  directory_bytes = sizeof(MOVIE_STREAM_DIRECTORY) - 1;
  name_bytes = strlen(stream_name);
  if(root_bytes + directory_bytes >= path_capacity ||
     name_bytes >= path_capacity - root_bytes - directory_bytes)
    {
      return 0;
    }

  memcpy(path, resource_root_path, root_bytes);
  memcpy(path + root_bytes, MOVIE_STREAM_DIRECTORY, directory_bytes);
  memcpy(path + root_bytes + directory_bytes, stream_name, name_bytes + 1);
  return 1;
}


void
play_async_movie_stream(const char *stream_name,
                        int         options)
{
  RwTagArgument tags[MOVIE_TAG_COUNT];
  char path[MOVIE_STREAM_PATH_BYTES];
  Item timer_item;
  int status;
  unsigned int buttons;

  if(retained_movie_stream_state != 0)
    {
      if(stop_retained_movie_stream() < 0)
        {
          return;
        }
    }

  tags[MOVIE_TAG_AUDIO_CHUNK_CALLBACK_INDEX].tag =
    ASYNC_STREAM_TAG_AUDIO_CHUNK_CALLBACK;
  tags[MOVIE_TAG_AUDIO_INITIALIZE_CALLBACK_INDEX].tag =
    ASYNC_STREAM_TAG_AUDIO_INITIALIZE_CALLBACK;
  if((options & MOVIE_OPTION_ASYNCHRONOUS) != 0)
    {
      tags[MOVIE_TAG_AUDIO_CHUNK_CALLBACK_INDEX].argument = 0;
      tags[MOVIE_TAG_AUDIO_INITIALIZE_CALLBACK_INDEX].argument = 0;
    }
  else
    {
      tags[MOVIE_TAG_AUDIO_CHUNK_CALLBACK_INDEX].argument =
        (unsigned int)submit_stream_audio_chunk;
      tags[MOVIE_TAG_AUDIO_INITIALIZE_CALLBACK_INDEX].argument =
        (unsigned int)initialize_stream_audio_playback;
    }

  tags[MOVIE_TAG_FIRST_SCREEN_INDEX].tag =
    ASYNC_STREAM_TAG_FIRST_SCREEN_ITEM;
  tags[MOVIE_TAG_FIRST_SCREEN_INDEX].argument =
    display_runtime_globals.screen_items[
      1 - display_runtime_globals.active_bitmap_index];
  tags[MOVIE_TAG_SECOND_SCREEN_INDEX].tag =
    ASYNC_STREAM_TAG_SECOND_SCREEN_ITEM;
  tags[MOVIE_TAG_SECOND_SCREEN_INDEX].argument =
    display_runtime_globals.screen_items[
      display_runtime_globals.active_bitmap_index];
  tags[MOVIE_TAG_FRAME_X_INDEX].tag = ASYNC_STREAM_TAG_FRAME_X;
  tags[MOVIE_TAG_FRAME_X_INDEX].argument = (unsigned int)-1;
  tags[MOVIE_TAG_BUFFER_GROUP_INDEX].tag =
    ASYNC_STREAM_TAG_BUFFER_GROUP_SIZE;
  tags[MOVIE_TAG_BUFFER_GROUP_INDEX].argument = 1;
  tags[MOVIE_TAG_FRAME_Y_INDEX].tag = ASYNC_STREAM_TAG_FRAME_Y;
  if((options & MOVIE_OPTION_DECODE_LIMIT) != 0)
    {
      tags[MOVIE_TAG_FRAME_Y_INDEX].argument = MOVIE_DECODE_LIMIT;
    }
  else
    {
      tags[MOVIE_TAG_FRAME_Y_INDEX].argument = (unsigned int)-1;
    }
  tags[MOVIE_TAG_TERMINATOR_INDEX].tag = 0;
  tags[MOVIE_TAG_TERMINATOR_INDEX].argument = 0;

  if(!build_movie_stream_path(path, sizeof(path), stream_name))
    {
      return;
    }

  status = open_async_stream(&retained_movie_stream_state, path,
                             MOVIE_STREAM_UNIT_COUNT, tags);
  if(status != 0)
    {
      return;
    }

  do
    {
      sample_stream_input_pads();
    } while(get_stream_input_buttons(1) != 0);

  tags[MOVIE_PLAYBACK_TAG_WAIT_INDEX].tag =
    ASYNC_STREAM_TAG_WAIT_FOR_BUFFER;
  tags[MOVIE_PLAYBACK_TAG_WAIT_INDEX].argument = 1;
  tags[MOVIE_PLAYBACK_TAG_TERMINATOR_INDEX].tag = 0;
  tags[MOVIE_PLAYBACK_TAG_TERMINATOR_INDEX].argument = 0;

  if((options & (MOVIE_OPTION_FADE_CUE | MOVIE_OPTION_FADE_NOW)) != 0)
    {
      handle_movie_frame_transition_cue(0, MOVIE_FADE_TIME);
    }
  if((options & MOVIE_OPTION_PRESERVE_DISPLAY) == 0)
    {
      flip_async_movie_display_pair();
    }

  status = start_async_stream_playback(retained_movie_stream_state, tags);
  if(status != 0)
    {
      stop_retained_movie_stream();
      return;
    }

  timer_item = CreateSizedItem(MOVIE_TIMER_IOREQ_NODE_ID, 0, 0);
  if(timer_item < 0)
    {
      stop_retained_movie_stream();
      return;
    }
  status = poll_async_stream_completion(retained_movie_stream_state, 0);
  while(status == 0)
    {
      sample_stream_input_pads();
      buttons = get_stream_input_buttons(1);
      if((buttons & MOVIE_EXIT_BUTTON_MASK) != 0)
        {
          break;
        }
      SleepUntilTime(timer_item,
                     GetAudioTime() + movie_playback_control.frame_delay);
      status = poll_async_stream_completion(retained_movie_stream_state, 0);
    }

  DeleteItem(timer_item);
  stop_retained_movie_stream();
  if((options & MOVIE_OPTION_CLEAR_AFTER_PLAYBACK) != 0)
    {
      flip_async_movie_display_pair();
    }
}

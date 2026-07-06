#include "string.h"

#include "audio.h"
#include "stdlib.h"
/* Reconstructed from the original function at 0x000333A4. */



#include "async_stream_runtime.h"
#include "path_runtime.h"
#include "rw_semantic_data.h"

typedef struct Div10Result
{
  unsigned int quotient;
  unsigned int remainder;
} Div10Result;

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;

#define AUDIO_FLAG_A runtime_feature_flags.game_audio_enabled.value
#define AUDIO_FLAG_B runtime_feature_flags.background_audio_enabled.value
#define BACKGROUND_AUDIO_PATH_BYTES 68

__value_in_regs Div10Result
unsigned_divide_by_ten_with_remainder(unsigned int numerator);
__value_in_regs DivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
int
initialize_background_audio_music_context(void);
int
open_async_stream(AsyncStreamPlaybackStatePrefix **state,
                  const char                      *path,
                  int                              stream_unit_count,
                  const RwTagArgument             *tags);
int
initialize_default_music_context(void);
int
start_async_stream_playback(AsyncStreamPlaybackStatePrefix *state,
                            const RwTagArgument            *tags);

int
start_next_background_audio_stream(void)
{
  BackgroundAudioGlobals *ctrl;
  unsigned int count;
  unsigned int index;
  int value;
  char path[BACKGROUND_AUDIO_PATH_BYTES];
  int result;
  int stop_accepted;

  if(AUDIO_FLAG_A == 0 || AUDIO_FLAG_B == 0)
    {
      return 0;
    }

  ctrl = &background_audio_globals;
  if(ctrl->stream_state != 0)
    {
      return -1;
    }
  if(ctrl->name_count <= BACKGROUND_AUDIO_NO_TRACKS)
    {
      return 0;
    }

  if(ctrl->name_count == BACKGROUND_AUDIO_SINGLE_TRACK)
    {
      race_mode_state.background_audio_index = 0;
      race_mode_state.previous_background_audio_index = 0;
    }
  else
    {
      race_mode_state.previous_background_audio_index =
        race_mode_state.background_audio_index;

      do
        {
          index = 0;
          count = unsigned_divide_by_ten_with_remainder((unsigned int)GetAudioTime()).remainder;
          while(count > index)
            {
              value = rand();
              race_mode_state.background_audio_index =
                (signed char)signed_divide_with_remainder(
                  (unsigned int)ctrl->name_count,
                  (unsigned int)value).remainder;
              index += 1;
              count = unsigned_divide_by_ten_with_remainder(
                (unsigned int)GetAudioTime()).remainder;
            }
        } while(race_mode_state.previous_background_audio_index ==
                race_mode_state.background_audio_index);
    }

  result = initialize_background_audio_music_context();
  if(result < 0)
    {
      initialize_default_music_context();
      return result;
    }
  if(copy_path_with_capacity(path, sizeof(path), resource_root_path) < 0 ||
     append_path_with_capacity(
       path, sizeof(path), "Streams/BGAudio/") < 0 ||
     append_path_with_capacity(
       path, sizeof(path),
       (char *)ctrl->names[
         race_mode_state.previous_background_audio_index]) < 0)
    {
      initialize_default_music_context();
      ctrl->stream_state = 0;
      return -1;
    }

  result = open_async_stream(
    (AsyncStreamPlaybackStatePrefix **)&ctrl->stream_state, path,
    ctrl->stream_argument, background_stream_tags);
  if(result != 0)
    {
      initialize_default_music_context();
      ctrl->stream_state = 0;
      return result;
    }

  result = ctrl->current_gain;
  background_command_tags[0].argument = (unsigned int)result;
  result = start_async_stream_playback(
    (AsyncStreamPlaybackStatePrefix *)ctrl->stream_state,
    background_command_tags);
  if(result < 0)
    {
      stop_async_stream(
        (AsyncStreamPlaybackStatePrefix *)ctrl->stream_state,
        &stop_accepted);
      if(stop_accepted != 0)
        {
          ctrl->stream_state = 0;
          initialize_default_music_context();
        }
    }
  return result;
}

#ifndef ROADRASH_MOVIE_PLAYBACK_RUNTIME_H
#define ROADRASH_MOVIE_PLAYBACK_RUNTIME_H

enum MoviePlaybackOption
{
  MOVIE_OPTION_ASYNCHRONOUS = 0x01,
  MOVIE_OPTION_FADE_CUE = 0x02,
  MOVIE_OPTION_FADE_NOW = 0x04,
  MOVIE_OPTION_PRESERVE_DISPLAY = 0x16,
  MOVIE_OPTION_CLEAR_AFTER_PLAYBACK = 0x20,
  MOVIE_OPTION_DECODE_LIMIT = 0x40
};

enum MoviePlaybackConstant
{
  MOVIE_STREAM_PATH_BYTES = 0x60,
  MOVIE_STREAM_UNIT_COUNT = 5,
  MOVIE_FADE_TIME = -0xF0,
  MOVIE_DECODE_LIMIT = 10,
  MOVIE_PREROLL_WAIT_FIELDS = 60,
  MOVIE_SIGN_BIT_SHIFT = 31,
  MOVIE_BUFFER_COUNT_UNAVAILABLE = -1000,
  MOVIE_TIMER_IOREQ_NODE_ID = 0x405
};

#define MOVIE_EXIT_BUTTON_MASK 0x0E000000U
#define MOVIE_STREAM_DIRECTORY "Streams/"

void
play_async_movie_stream(const char *stream_name,
                        int         options);
void
enable_movie_playback(void);
int
initialize_movie_subscribers(void);
void
play_movie_stream(const char *stream_name,
                  int32       options);
int
dispose_movie_subscribers(void);
int
release_movie_playback_resources(void);
int
stop_retained_movie_stream(void);

#endif

#ifndef ASYNC_STREAM_PLAYBACK_HELPERS_H
#define ASYNC_STREAM_PLAYBACK_HELPERS_H

#include "codec.h"

#include "async_stream_runtime.h"
#include "stream_audio_playback.h"

enum
{
  ASYNC_STREAM_FRAME_COPY_PAGE_COUNT = 75
};

typedef struct AsyncStreamAudioPlaybackContext
{
  AsyncStreamPlaybackStatePrefix *state;
  AsyncStreamAudioInitializeCallback initialize_callback;
  AsyncStreamAudioSubmitCallback submit_callback;
  unsigned int scheduled_end_time;
  unsigned char awaiting_first_sample;
  unsigned char alignment[3];
  int channel_count;
} AsyncStreamAudioPlaybackContext;

typedef struct AsyncStreamFilmPlaybackContext
{
  AsyncStreamPlaybackStatePrefix *state;
  int horizontal_position;
  int vertical_position;
  Bitmap *bitmap;
  int row_bytes;
  unsigned char *destination;
  codec film_codec;
  CCB *cel_list;
} AsyncStreamFilmPlaybackContext;

typedef char AsyncStreamChunkHeaderSizeIs0x14[
    sizeof(AsyncStreamChunkHeader) == 0x14 ? 1 : -1];
typedef char AsyncStreamAudioPlaybackContextSizeIs0x18[
    sizeof(AsyncStreamAudioPlaybackContext) == 0x18 ? 1 : -1];
typedef char AsyncStreamFilmPlaybackContextSizeIs0x20[
    sizeof(AsyncStreamFilmPlaybackContext) == 0x20 ? 1 : -1];

void
present_pending_async_stream_frame(const AsyncStreamChunkHeader   *next_chunk,
                                   AsyncStreamPlaybackStatePrefix *state);
void
process_async_stream_film_chunk(AsyncStreamChunkHeader         *chunk,
                                AsyncStreamFilmPlaybackContext *film);
void
process_async_stream_audio_chunk(const AsyncStreamChunkHeader    *chunk,
                                 AsyncStreamAudioPlaybackContext *audio);

#endif

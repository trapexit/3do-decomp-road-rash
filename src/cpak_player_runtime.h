#ifndef CPAK_PLAYER_RUNTIME_H
#define CPAK_PLAYER_RUNTIME_H

#ifndef offsetof
  #include "stddef.h"
#endif

#include "datastream.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "dsstreamheader.h"
#include "graphics.h"
#include "item.h"

#include "control_subscriber_runtime.h"
#include "cpak_subscriber_runtime.h"
#include "saudio_subscriber_runtime.h"
#include "stream_acquisition_runtime.h"

#define ROADRASH_CPAK_PLAYER_BYTES 0x130
#define ROADRASH_MOVIE_REQUEST_BYTES 0x80
#define ROADRASH_MOVIE_SCREEN_CONTEXT_BYTES 0x58
#define ROADRASH_MOVIE_SCREEN_CAPACITY 6

typedef struct RoadRashMovieScreenContext
{
  int32 screen_count;
  int32 current_screen;
  int32 frame_buffer_pages;
  int32 frame_buffer_byte_count;
  Item screen_items[ROADRASH_MOVIE_SCREEN_CAPACITY];
  Item bitmap_items[ROADRASH_MOVIE_SCREEN_CAPACITY];
  Bitmap *bitmaps[ROADRASH_MOVIE_SCREEN_CAPACITY];
} RoadRashMovieScreenContext;

typedef struct RoadRashMoviePlaybackRequest
{
  unsigned char volume_controls_enabled;
  unsigned char volume_controls_alignment[3];
  int32 audio_channel;
  int32 reserved;
  int32 cue_request_count;
  RoadRashControlCueRequest frame_transition_cue;
  RoadRashMovieScreenContext screen_context;
} RoadRashMoviePlaybackRequest;

struct RoadRashCPakPlayer;
typedef int32 (*RoadRashCPakPlayerCallback)(struct RoadRashCPakPlayer    *player,
                                            RoadRashMoviePlaybackRequest *request);

typedef struct RoadRashCPakPlayer
{
  RoadRashCPakPlayerCallback callback;
  RoadRashMoviePlaybackRequest *callback_request;
  DSHeaderChunk header;
  DSDataBufPtr buffer_list;
  RoadRashDataAcquisitionContextPtr acquisition_context;
  DSStreamCBPtr stream_context;
  RoadRashMovieScreenContext *screen_context;
  Item vbl_io_request;
  Item vram_io_request;
  Item reply_port;
  Item request_message;
  Item end_of_stream_message;
  RoadRashControlContext *control_context;
  SAudioContext *audio_context;
  CPakContextPtr cpak_context;
  CPakRecPtr cpak_record;
} RoadRashCPakPlayer;

typedef char RoadRashMovieScreenContextSizeCheck[
    sizeof(RoadRashMovieScreenContext) ==
    ROADRASH_MOVIE_SCREEN_CONTEXT_BYTES
        ? 1 : -1];
typedef char RoadRashMovieScreenCurrentOffsetCheck[
    offsetof(RoadRashMovieScreenContext, current_screen) == 0x04 ? 1 : -1];
typedef char RoadRashMovieScreenItemsOffsetCheck[
    offsetof(RoadRashMovieScreenContext, screen_items) == 0x10 ? 1 : -1];
typedef char RoadRashMovieBitmapItemsOffsetCheck[
    offsetof(RoadRashMovieScreenContext, bitmap_items) == 0x28 ? 1 : -1];
typedef char RoadRashMovieBitmapsOffsetCheck[
    offsetof(RoadRashMovieScreenContext, bitmaps) == 0x40 ? 1 : -1];
typedef char RoadRashMovieRequestCueCountOffsetCheck[
    offsetof(RoadRashMoviePlaybackRequest, cue_request_count) == 0x0C
        ? 1 : -1];
typedef char RoadRashMovieRequestCueOffsetCheck[
    offsetof(RoadRashMoviePlaybackRequest, frame_transition_cue) == 0x10
        ? 1 : -1];
typedef char RoadRashMovieRequestScreenOffsetCheck[
    offsetof(RoadRashMoviePlaybackRequest, screen_context) == 0x28
        ? 1 : -1];
typedef char RoadRashMovieRequestSizeCheck[
    sizeof(RoadRashMoviePlaybackRequest) == ROADRASH_MOVIE_REQUEST_BYTES
        ? 1 : -1];
typedef char RoadRashCPakHeaderOffsetCheck[
    offsetof(RoadRashCPakPlayer, header) == 0x08 ? 1 : -1];
typedef char RoadRashCPakBufferListOffsetCheck[
    offsetof(RoadRashCPakPlayer, buffer_list) == 0xFC ? 1 : -1];
typedef char RoadRashCPakAcquisitionOffsetCheck[
    offsetof(RoadRashCPakPlayer, acquisition_context) == 0x100 ? 1 : -1];
typedef char RoadRashCPakStreamOffsetCheck[
    offsetof(RoadRashCPakPlayer, stream_context) == 0x104 ? 1 : -1];
typedef char RoadRashCPakScreenOffsetCheck[
    offsetof(RoadRashCPakPlayer, screen_context) == 0x108 ? 1 : -1];
typedef char RoadRashCPakVblRequestOffsetCheck[
    offsetof(RoadRashCPakPlayer, vbl_io_request) == 0x10C ? 1 : -1];
typedef char RoadRashCPakVramRequestOffsetCheck[
    offsetof(RoadRashCPakPlayer, vram_io_request) == 0x110 ? 1 : -1];
typedef char RoadRashCPakReplyPortOffsetCheck[
    offsetof(RoadRashCPakPlayer, reply_port) == 0x114 ? 1 : -1];
typedef char RoadRashCPakRequestMessageOffsetCheck[
    offsetof(RoadRashCPakPlayer, request_message) == 0x118 ? 1 : -1];
typedef char RoadRashCPakEndMessageOffsetCheck[
    offsetof(RoadRashCPakPlayer, end_of_stream_message) == 0x11C ? 1 : -1];
typedef char RoadRashCPakControlOffsetCheck[
    offsetof(RoadRashCPakPlayer, control_context) == 0x120 ? 1 : -1];
typedef char RoadRashCPakAudioOffsetCheck[
    offsetof(RoadRashCPakPlayer, audio_context) == 0x124 ? 1 : -1];
typedef char RoadRashCPakContextOffsetCheck[
    offsetof(RoadRashCPakPlayer, cpak_context) == 0x128 ? 1 : -1];
typedef char RoadRashCPakRecordOffsetCheck[
    offsetof(RoadRashCPakPlayer, cpak_record) == 0x12C ? 1 : -1];
typedef char RoadRashCPakPlayerSizeCheck[
    sizeof(RoadRashCPakPlayer) == ROADRASH_CPAK_PLAYER_BYTES ? 1 : -1];

/* EOF request storage must survive until stream teardown completes. */
int32
play_prepared_cpak_stream(RoadRashCPakPlayer           *player,
                          RoadRashMovieScreenContext   *screen_context,
                          DSRequestMsgPtr              end_of_stream_request,
                          int32                         horizontal_offset,
                          int32                         vertical_offset,
                          RoadRashCPakPlayerCallback    callback,
                          RoadRashMoviePlaybackRequest *callback_request);
int32
prepare_cpak_player(RoadRashCPakPlayer *player,
                    char               *stream_path,
                    uint32              include_audio);
int32
finish_preparing_cpak_player_audio(RoadRashCPakPlayer *player);
int32
dismantle_cpak_player(RoadRashCPakPlayer *player);

DSDataBufPtr
create_data_stream_buffer_list(long buffer_count,
                               long buffer_size);
int32
dispose_data_stream_buffer_list(DSDataBufPtr buffer_list);

int32
service_movie_stream_playback(RoadRashCPakPlayer           *player,
                              RoadRashMoviePlaybackRequest *request);
void
handle_movie_frame_transition_cue(void *unused_context,
                                  int32 relative_stream_time);

#endif

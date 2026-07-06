#ifndef ASYNC_STREAM_RUNTIME_H
#define ASYNC_STREAM_RUNTIME_H

#include "filesystem.h"

#include "rw_semantic_data.h"

typedef struct AsyncStreamBuffer
{
  int state;
  int block_offset;
  int io_request;
  IOReq *io_request_record;
  void *allocation_base;
  unsigned char *data;
} AsyncStreamBuffer;

typedef struct AsyncBlockStream
{
  int next_block;
  int buffer_count;
  int blocks_per_buffer;
  int file_item;
  FileStatus file_status;
  unsigned char buffering_enabled;
  unsigned char buffering_enabled_alignment[3];
  int last_error;
  AsyncStreamBuffer *buffers;
} AsyncBlockStream;

enum AsyncStreamChunkType
{
  ASYNC_STREAM_CHUNK_TYPE_FILM = 0x46494C4DU,
  ASYNC_STREAM_CHUNK_TYPE_FILL = 0x46494C4CU,
  ASYNC_STREAM_CHUNK_TYPE_STREAMED_ANIMATION = 0x53414E4DU,
  ASYNC_STREAM_CHUNK_TYPE_SOUND = 0x534E4453U,
  ASYNC_STREAM_CHUNK_TYPE_CONTROL = 0x4354524CU
};

enum AsyncStreamBufferState
{
  ASYNC_STREAM_BUFFER_FREE = 0,
  ASYNC_STREAM_BUFFER_READ_PENDING = 1,
  ASYNC_STREAM_BUFFER_READY = 2,
  ASYNC_STREAM_BUFFER_ACQUIRED = 3,
  ASYNC_STREAM_BUFFER_SHORT_READ = 4
};

enum AsyncBlockStreamStatus
{
  ASYNC_BLOCK_STREAM_ERROR_BAD_POINTER = -3,
  ASYNC_STREAM_ERROR_INVALID_CHUNK = -4,
  ASYNC_BLOCK_STREAM_STATUS_END_OF_STREAM = 1,
  ASYNC_BLOCK_STREAM_STATUS_BUFFERING = 2
};

enum AsyncBlockStreamBufferResult
{
  ASYNC_BLOCK_STREAM_NO_BUFFER = -1,
  ASYNC_BLOCK_STREAM_MORE_DATA_PENDING = -2
};

enum
{
  ASYNC_BLOCK_STREAM_BLOCK_SHIFT = 11
};

typedef struct AsyncStreamChunkHeader
{
  unsigned int chunk_type;
  int chunk_size;
  unsigned int stream_time;
  int channel;
  unsigned int subchunk_type;
} AsyncStreamChunkHeader;

enum AsyncStreamControlSubtype
{
  ASYNC_STREAM_CONTROL_SUBTYPE_CUES = 0x43554553U,
  ASYNC_STREAM_CONTROL_SUBTYPE_SYNC = 0x53594E43U
};

typedef struct AsyncStreamControlChunk
{
  unsigned int chunk_type;
  int chunk_size;
  unsigned int stream_time;
  int channel;
  unsigned int subchunk_type;
  unsigned int control_value;
} AsyncStreamControlChunk;

enum AsyncStreamTag
{
  ASYNC_STREAM_TAG_IO_BLOCK_BYTES = 0x00600000,
  ASYNC_STREAM_TAG_BUFFER_GROUP_SIZE = 0x00600001,
  ASYNC_STREAM_TAG_SUBTASK_PRIORITY_OFFSET = 0x00600002,
  ASYNC_STREAM_TAG_FRAME_X = 0x00600003,
  ASYNC_STREAM_TAG_FRAME_Y = 0x00600004,
  ASYNC_STREAM_TAG_RETAINED_EXTENSION_ONE = 0x00600005,
  ASYNC_STREAM_TAG_FIRST_SCREEN_ITEM = 0x00600006,
  ASYNC_STREAM_TAG_SECOND_SCREEN_ITEM = 0x00600007,
  ASYNC_STREAM_TAG_FRAME_BITMAP = 0x00600008,
  ASYNC_STREAM_TAG_FRAME_CEL_LIST = 0x00600009,
  ASYNC_STREAM_TAG_RETAINED_EXTENSION_TWO = 0x0060000A,
  ASYNC_STREAM_TAG_AUDIO_INITIALIZE_CALLBACK = 0x0060000B,
  ASYNC_STREAM_TAG_AUDIO_CHUNK_CALLBACK = 0x0060000C,
  ASYNC_STREAM_TAG_AUDIO_AMPLITUDE = 0x0060000D,
  ASYNC_STREAM_TAG_WAIT_FOR_BUFFER = 0x00600010
};

enum AsyncStreamCommand
{
  ASYNC_STREAM_COMMAND_STARTUP = 1,
  ASYNC_STREAM_COMMAND_PLAY = 2,
  ASYNC_STREAM_COMMAND_CONTROL = 3,
  ASYNC_STREAM_COMMAND_PAUSE = 4,
  ASYNC_STREAM_COMMAND_REPEAT = 5,
  ASYNC_STREAM_COMMAND_BUFFERING = 6,
  ASYNC_STREAM_COMMAND_STOP = 7
};

enum BackgroundAudioTrackCount
{
  BACKGROUND_AUDIO_NO_TRACKS = 0,
  BACKGROUND_AUDIO_SINGLE_TRACK = 1
};

enum AsyncStreamBufferingFlags
{
  ASYNC_STREAM_BUFFERING_FLAG_ENABLE = 1,
  ASYNC_STREAM_BUFFERING_FLAG_WAIT = 2
};

enum AsyncStreamBufferingState
{
  ASYNC_STREAM_BUFFERING_ENABLED = 0,
  ASYNC_STREAM_BUFFERING_DISABLED = 1,
  ASYNC_STREAM_BUFFERING_ENABLE_PENDING = 2,
  ASYNC_STREAM_BUFFERING_DISABLE_PENDING = 3
};

#define ASYNC_STREAM_UPDATE_STATE_ONLY ((int *)-1)

enum AsyncStreamDispatchStatus
{
  ASYNC_STREAM_DISPATCH_STOP = 0,
  ASYNC_STREAM_DISPATCH_CONTINUE = 1,
  ASYNC_STREAM_DISPATCH_INVALID_MESSAGE = 2
};

enum
{
  ASYNC_STREAM_PLAYBACK_TAG_CAPACITY = 17,
  ASYNC_STREAM_PLAYBACK_STATE_BYTES = 0x160,
  ASYNC_STREAM_THREAD_STACK_BYTES = 0x1000,
  ASYNC_STREAM_OPEN_THREAD_NAME_BYTES = 0x400,
  ASYNC_STREAM_OBJECT_NAME_BYTES = 0x100,
  ASYNC_STREAM_FALLBACK_THREAD_NAME_BYTES = 15,
  ASYNC_STREAM_MESSAGE_PRIORITY = 0x80,
  ASYNC_STREAM_AUDIO_AMPLITUDE_MAX = 0x7FFF,
  ASYNC_STREAM_COMPLETION_SLOT_COUNT = 6,
  ASYNC_STREAM_THREAD_POLL_TICKS = 10
};

typedef struct AsyncStreamOpenRequest
{
  const char *path;
  int stream_unit_count;
  const RwTagArgument *tags;
  void *thread_stack_block;
} AsyncStreamOpenRequest;

/* Understood prefix of the 0x160-byte playback-state allocation. */
typedef struct AsyncStreamPlaybackStatePrefix
{
  int stream_unit_count;
  int io_block_bytes;
  int buffer_group_size;
  unsigned char active;
  unsigned char repeat;
  unsigned char alignment[2];
  int control_value;
  AsyncBlockStream *block_stream;
  RwTagArgument tag_overrides[ASYNC_STREAM_PLAYBACK_TAG_CAPACITY];
  Item command_port_item;
  Item command_message_item;
  Item buffering_semaphore_item;
  int buffering_state;
  Item playback_semaphore_item;
  unsigned int audio_clock_origin;
  unsigned int scheduled_frame_time;
  Item video_copy_io_request_item;
  Bitmap *first_screen_bitmap;
  Bitmap *second_screen_bitmap;
  Item first_screen_item;
  Item second_screen_item;
  unsigned char frame_copy_pending;
  unsigned char frame_copy_alignment[3];
  Item audio_cue_item;
  int requested_audio_amplitude;
  int applied_audio_amplitude;
  Item playback_thread_item;
  void *thread_stack_block;
} AsyncStreamPlaybackStatePrefix;

typedef struct AsyncStreamCompletionSlot
{
  Item pending_item;
  int reserved[3];
} AsyncStreamCompletionSlot;

/* View of the playback-state fields consulted by completion polling. */
typedef struct AsyncStreamCompletionStateView
{
  AsyncStreamPlaybackStatePrefix prefix;
  int reserved_after_prefix;
  AsyncStreamCompletionSlot slots[ASYNC_STREAM_COMPLETION_SLOT_COUNT];
} AsyncStreamCompletionStateView;

void
apply_stream_tag_overrides(RwTagArgument       *defaults,
                           const RwTagArgument *overrides);
int
initialize_async_stream_message_port(const RwTagArgument *tag_overrides);
const
RwTagArgument *
find_stream_tag_argument(unsigned int         tag,
                         const RwTagArgument *overrides,
                         const RwTagArgument *defaults);
void
upsert_stream_tag_argument(RwTagArgument       *entries,
                           int                  capacity,
                           const RwTagArgument *incoming);
void
merge_stream_playback_tag_overrides(AsyncStreamPlaybackStatePrefix *state,
                                    const RwTagArgument            *incoming);
void
apply_stream_control_clock_sync(const AsyncStreamControlChunk  *chunk,
                                AsyncStreamPlaybackStatePrefix *state);
AsyncBlockStream *
allocate_async_block_stream_storage(int buffer_byte_count,
                                    int requested_buffer_count);
int
dispose_async_block_stream_storage(AsyncBlockStream *stream);
AsyncBlockStream *
create_async_block_stream(const char *path,
                          int         buffer_byte_count,
                          int         buffer_count,
                          int         wait_for_buffer);
int
dispose_async_block_stream(AsyncBlockStream *stream);
int
dispose_async_stream_playback_resources(AsyncStreamPlaybackStatePrefix *state);
AsyncStreamPlaybackStatePrefix *
create_async_stream_playback_state(RwMemoryAllocator             allocate,
                                   const AsyncStreamOpenRequest *request);
int
release_retained_async_stream_open_resources(void);
void
async_stream_playback_thread_main(int  unused_initial_argument,
                                  Item parent_port_item);
int
service_async_stream_messages(AsyncStreamPlaybackStatePrefix *state,
                              unsigned char                  *paused,
                              unsigned char                  *continue_playback,
                              int                             yield_when_idle,
                              int                             refresh_audio_amplitude);
int
dispatch_async_stream_command(AsyncStreamPlaybackStatePrefix *state,
                              Item                            message_item,
                              int                             from_playback_loop,
                              unsigned char                  *paused);
int
run_async_stream_playback(AsyncStreamPlaybackStatePrefix *state);
void
adjust_async_stream_gain(AsyncStreamPlaybackStatePrefix *state,
                         int                             amplitude_delta);
int
no_op_async_stream_poll(AsyncStreamPlaybackStatePrefix *stream_state);
int
poll_async_stream_completion(AsyncStreamPlaybackStatePrefix *state,
                             int                             wait_for_completion);
int
stop_async_stream(AsyncStreamPlaybackStatePrefix *state,
                  int                            *stop_accepted);
int
seek_async_block_stream(AsyncBlockStream *stream,
                        int               block_number,
                        int               wait_for_buffer);
int
queue_async_block_stream_reads(AsyncBlockStream *stream,
                               int               wait_for_buffer);
int
service_async_block_stream_reads(AsyncBlockStream *stream);
void
select_oldest_async_block_stream_buffers(AsyncBlockStream *stream,
                                         int              *ready_index,
                                         int              *pending_index);
int
wait_for_next_async_block_stream_buffer(AsyncBlockStream *stream);
int
read_async_block_stream_buffer(AsyncBlockStream *stream,
                               void             *destination);
int
poll_async_block_stream_readiness(AsyncBlockStream *stream);
unsigned char *
acquire_async_block_stream_chunk(AsyncBlockStream *stream);
int
release_async_block_stream_chunk(AsyncBlockStream *stream,
                                 const void       *chunk);
int
get_async_block_stream_status(const AsyncBlockStream *stream);
int
set_async_block_stream_buffering(AsyncBlockStream *stream,
                                 int               buffering_enabled,
                                 int               wait_for_buffer);
int
set_async_stream_buffering_mode(int  stream_handle,
                                int  buffering_enabled,
                                int  wait_for_change,
                                int *buffer_count_out);
int
stop_background_audio_stream_if_active(void);

#endif

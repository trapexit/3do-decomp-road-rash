#ifndef SAUDIO_SUBSCRIBER_RUNTIME_H
#define SAUDIO_SUBSCRIBER_RUNTIME_H

#include "datastream.h"
#include "mempool.h"

#include "platform.h"
#include "rw_semantic_data.h"

#define SAUDIO_SUBSCRIBER_CHANNEL_COUNT 8
#define SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number) \
        ((unsigned int)(channel_number) < SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
#define SAUDIO_SUBSCRIBER_THREAD_STACK_BYTES 0x1000

#define SAUDIO_OUTPUT_SIZE 0x28
#define SAUDIO_OUTPUT_INSTRUMENT_OFFSET 0x00
#define SAUDIO_OUTPUT_CHANNEL_COUNT_OFFSET 0x04
#define SAUDIO_OUTPUT_LEFT_ENVELOPE_OFFSET 0x08
#define SAUDIO_OUTPUT_LEFT_TARGET_KNOB_OFFSET 0x0C
#define SAUDIO_OUTPUT_RIGHT_ENVELOPE_OFFSET 0x10
#define SAUDIO_OUTPUT_RIGHT_TARGET_KNOB_OFFSET 0x14
#define SAUDIO_OUTPUT_CURRENT_AMPLITUDE_OFFSET 0x18
#define SAUDIO_OUTPUT_SAVED_AMPLITUDE_OFFSET 0x1C
#define SAUDIO_OUTPUT_MUTED_OFFSET 0x20
#define SAUDIO_OUTPUT_EXTERNAL_MUTE_OFFSET 0x21
#define SAUDIO_OUTPUT_CURRENT_PAN_OFFSET 0x24
#define SAUDIO_CHANNEL_SIZE 0x54
#define SAUDIO_CHANNEL_STATUS_OFFSET 0x00
#define SAUDIO_CHANNEL_BUFFER_COUNT_OFFSET 0x04
#define SAUDIO_CHANNEL_BUFFER_POOL_OFFSET 0x08
#define SAUDIO_CHANNEL_INSTRUMENT_OFFSET 0x0C
#define SAUDIO_CHANNEL_INSTRUMENT_STARTED_OFFSET 0x10
#define SAUDIO_CHANNEL_ATTACHMENTS_RUNNING_OFFSET 0x11
#define SAUDIO_CHANNEL_SIGNAL_MASK_OFFSET 0x14
#define SAUDIO_CHANNEL_PENDING_MESSAGES_OFFSET 0x18
#define SAUDIO_CHANNEL_IN_USE_COUNT_OFFSET 0x20
#define SAUDIO_CHANNEL_IN_USE_QUEUE_HEAD_OFFSET 0x24
#define SAUDIO_CHANNEL_IN_USE_QUEUE_TAIL_OFFSET 0x28
#define SAUDIO_CHANNEL_OUTPUT_OFFSET 0x2C
#define SAUDIO_MESSAGE_SIZE 0x18
#define SAUDIO_MESSAGE_ITEM_OFFSET 0x04
#define SAUDIO_MESSAGE_LINK_OFFSET 0x0C
#define SAUDIO_MESSAGE_PAYLOAD_OFFSET 0x10
#define SAUDIO_MESSAGE_QUEUE_HEAD_OFFSET 0x00
#define SAUDIO_MESSAGE_QUEUE_TAIL_OFFSET 0x04
#define SAUDIO_CONTEXT_SIZE 0x2D4
#define SAUDIO_CONTEXT_STREAM_CONTROL_OFFSET 0x1C
#define SAUDIO_CONTEXT_TEMPLATE_ARRAY_OFFSET 0x20
#define SAUDIO_CONTEXT_BUFFER_SIGNALS_OFFSET 0x24
#define SAUDIO_CONTEXT_OUTPUT_TEMPLATE_OFFSET 0x28
#define SAUDIO_CONTEXT_ENVELOPE_TEMPLATE_OFFSET 0x2C
#define SAUDIO_CONTEXT_CLOCK_CHANNEL_OFFSET 0x30
#define SAUDIO_CONTEXT_CHANNELS_OFFSET 0x34
#define SAUDIO_BUFFER_SIZE 0x18
#define SAUDIO_BUFFER_SAMPLE_OFFSET 0x00
#define SAUDIO_BUFFER_ATTACHMENT_OFFSET 0x04
#define SAUDIO_BUFFER_CUE_OFFSET 0x08
#define SAUDIO_BUFFER_SIGNAL_OFFSET 0x0C
#define SAUDIO_BUFFER_PENDING_MESSAGE_OFFSET 0x10
#define SAUDIO_BUFFER_LINK_OFFSET 0x14
#define SAUDIO_BUFFER_INIT_BLOCK_SIZE 0x08
#define SAUDIO_BUFFER_INIT_CHANNEL_OFFSET 0x00
#define SAUDIO_BUFFER_INIT_HEADER_OFFSET 0x04
#define SAUDIO_CONTROL_BLOCK_SIZE 0x08
#define SAUDIO_HEADER_CHUNK_SIZE 0x40
#define SAUDIO_HEADER_CHANNEL_OFFSET 0x0C
#define SAUDIO_HEADER_VERSION_OFFSET 0x14
#define SAUDIO_HEADER_BUFFER_COUNT_OFFSET 0x18
#define SAUDIO_HEADER_INITIAL_AMPLITUDE_OFFSET 0x1C
#define SAUDIO_HEADER_INITIAL_PAN_OFFSET 0x20
#define SAUDIO_HEADER_SAMPLE_DESCRIPTOR_OFFSET 0x24
#define SAUDIO_SAMPLE_DESCRIPTOR_SIZE 0x1C
#define SAUDIO_SAMPLE_DESCRIPTOR_CHANNEL_COUNT_OFFSET 0x0C
#define SAUDIO_SAMPLE_CHUNK_SIZE 0x1C
#define SAUDIO_SAMPLE_CHUNK_STREAM_TIME_OFFSET 0x08
#define SAUDIO_SAMPLE_CHUNK_CHANNEL_OFFSET 0x0C
#define SAUDIO_SAMPLE_CHUNK_SAMPLE_BYTES_OFFSET 0x14
#define SAUDIO_SAMPLE_CHUNK_SAMPLES_OFFSET 0x18
#define SAUDIO_MEMORY_POOL_SIZE 0x10

#define SAUDIO_OUTPUT_INSTRUMENT_NAME "mixer2x2.dsp"
#define SAUDIO_ENVELOPE_INSTRUMENT_NAME "envelope.dsp"

#define SAUDIO_CHANNEL_ENABLED_FLAG 1U
#define SAUDIO_CHANNEL_ACTIVE_FLAG 2U
#define SAUDIO_CHANNEL_PLAYING_FLAGS                                      \
        (SAUDIO_CHANNEL_ENABLED_FLAG | SAUDIO_CHANNEL_ACTIVE_FLAG)
#define SAUDIO_CHANNEL_SYSTEM_BITS 0x0000FFFEU
#define SAUDIO_STREAM_FLUSH_OPTION 1U

#define SAUDIO_MONO_CHANNEL_COUNT 1
#define SAUDIO_STEREO_CHANNEL_COUNT 2
#define SAUDIO_INSTRUMENT_PRIORITY 100
#define SAUDIO_ENVELOPE_RAMP_INCREMENT 10
#define SAUDIO_AMPLITUDE_MAX 0x7FFF
#define SAUDIO_PAN_MAX 0x7FFF
#define SAUDIO_PAN_MIDPOINT 0x4000
#define SAUDIO_PAN_FULL_SCALE 0x8000
#define SAUDIO_PAN_FIXED_SHIFT 14
#define SAUDIO_CUE_ITEM_NODE 0x405

#define SAUDIO_AUDIO_TAG_NUM_BYTES 0x23
#define SAUDIO_AUDIO_TAG_ADDRESS 0x24
#define SAUDIO_AUDIO_TAG_ATTACHMENT_FLAGS 0x28
#define SAUDIO_AUDIO_TAG_SAMPLE_RATE 0x2E
#define SAUDIO_AUDIO_TAG_COMPRESSION_RATIO 0x2F
#define SAUDIO_AUDIO_TAG_COMPRESSION_TYPE 0x30
#define SAUDIO_AUDIO_TAG_SAMPLE_WIDTH 0x31
#define SAUDIO_AUDIO_TAG_CHANNEL_COUNT 0x17

#define SAUDIO_DYNAMIC_BUFFER_TAG_COUNT 3
#define SAUDIO_DYNAMIC_BUFFER_ADDRESS_INDEX 0
#define SAUDIO_DYNAMIC_BUFFER_SIZE_INDEX 1
#define SAUDIO_DYNAMIC_BUFFER_END_INDEX 2

#define SAUDIO_SAMPLE_FORMAT_TAG_COUNT 6
#define SAUDIO_SAMPLE_CHANNEL_TAG_INDEX 0
#define SAUDIO_SAMPLE_WIDTH_TAG_INDEX 1
#define SAUDIO_SAMPLE_RATE_TAG_INDEX 2
#define SAUDIO_SAMPLE_RATIO_TAG_INDEX 3
#define SAUDIO_SAMPLE_COMPRESSION_TAG_INDEX 4
#define SAUDIO_SAMPLE_END_TAG_INDEX 5

#define SAUDIO_SAMPLE_RATE_22KHZ 22050
#define SAUDIO_SAMPLE_RATE_44KHZ 44100
#define SAUDIO_SAMPLE_WIDTH_8_BITS 8
#define SAUDIO_COMPRESSION_SDX2 0x53445832

#define SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_MONO 0x01010101
#define SAUDIO_TEMPLATE_TAG_44KHZ_8BIT_MONO 0x01010102
#define SAUDIO_TEMPLATE_TAG_22KHZ_8BIT_MONO 0x01010202
#define SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_STEREO 0x01020101
#define SAUDIO_TEMPLATE_TAG_44KHZ_8BIT_STEREO 0x01020102
#define SAUDIO_TEMPLATE_TAG_22KHZ_8BIT_STEREO 0x01020202
#define SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_MONO_SDX2 0x02010101
#define SAUDIO_TEMPLATE_TAG_22KHZ_16BIT_MONO_SDX2 0x02010201
#define SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_STEREO_SDX2 0x02020101
#define SAUDIO_TEMPLATE_TAG_22KHZ_16BIT_STEREO_SDX2 0x02020201

#define SAUDIO_ATTACHMENT_NO_AUTOSTART 1U
#define SAUDIO_ATTACHMENT_MONITOR_AT_END (-2)

#define SAUDIO_HEADER_CHUNK_TYPE 0x53484452U
#define SAUDIO_SAMPLE_CHUNK_TYPE 0x53534D50U
#define SAUDIO_RELEASED_SAMPLE_MARKER 0xDEADBEEFU

#define SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE (-6999)
#define SAUDIO_ERROR_AMPLITUDE_OUT_OF_RANGE (-6998)
#define SAUDIO_ERROR_PAN_OUT_OF_RANGE (-6997)
#define SAUDIO_ERROR_ALLOC_INSTRUMENT (-6996)
#define SAUDIO_ERROR_UNSUPPORTED_FORMAT (-6995)
#define SAUDIO_ERROR_CONNECT_INSTRUMENT (-6994)
#define SAUDIO_ERROR_GRAB_KNOB (-6993)
#define SAUDIO_ERROR_ALLOC_BUFFER_POOL (-6992)
#define SAUDIO_ERROR_AUDIO_CHANNEL_COUNT (-6991)
#define SAUDIO_ERROR_FREE_INSTRUMENT (-6990)
#define SAUDIO_ERROR_ALLOC_CONTEXT_POOL (-6989)
#define SAUDIO_ERROR_STREAM_VERSION (-6988)
#define SAUDIO_ERROR_INITIALIZE_BUFFER (-6987)
#define SAUDIO_ERROR_TEMPLATE_NOT_FOUND (-7000)
#define STREAM_ERROR_NO_MEMORY (-992)
#define STREAM_ERROR_NO_SIGNAL (-991)
#define STREAM_ERROR_SIGNAL (-990)

#define SAUDIO_INTERNAL_MUTE 0
#define SAUDIO_INTERNAL_UNMUTE 0
#define SAUDIO_USER_MUTE 1
#define SAUDIO_USER_UNMUTE 1

typedef enum SAudioStreamOperation
{
  SAUDIO_STREAM_OP_DATA = 0,
  SAUDIO_STREAM_OP_GET_CHANNEL,
  SAUDIO_STREAM_OP_SET_CHANNEL,
  SAUDIO_STREAM_OP_CONTROL,
  SAUDIO_STREAM_OP_SYNCHRONIZE,
  SAUDIO_STREAM_OP_START,
  SAUDIO_STREAM_OP_STOP,
  SAUDIO_STREAM_OP_OPENING,
  SAUDIO_STREAM_OP_CLOSING,
  SAUDIO_STREAM_OP_END_OF_FILE,
  SAUDIO_STREAM_OP_ABORT
} SAudioStreamOperation;

typedef enum SAudioControlOperation
{
  SAUDIO_CONTROL_LOAD_TEMPLATES = 0,
  SAUDIO_CONTROL_SET_AMPLITUDE,
  SAUDIO_CONTROL_SET_PAN,
  SAUDIO_CONTROL_SET_CLOCK_CHANNEL,
  SAUDIO_CONTROL_GET_AMPLITUDE,
  SAUDIO_CONTROL_GET_FREQUENCY,
  SAUDIO_CONTROL_GET_PAN,
  SAUDIO_CONTROL_GET_CLOCK_CHANNEL,
  SAUDIO_CONTROL_CLOSE_CHANNEL,
  SAUDIO_CONTROL_FLUSH_CHANNEL,
  SAUDIO_CONTROL_MUTE_CHANNEL,
  SAUDIO_CONTROL_UNMUTE_CHANNEL
} SAudioControlOperation;

typedef struct SAudioSubscriberMessage SAudioSubscriberMessage;
typedef struct SAudioBuffer SAudioBuffer;

typedef struct SAudioMessageQueue
{
  SAudioSubscriberMessage *head;
  SAudioSubscriberMessage *tail;
} SAudioMessageQueue;

struct SAudioBuffer
{
  Item sample;
  Item attachment;
  Item cue;
  unsigned int signal;
  SAudioSubscriberMessage *pending_message;
  SAudioBuffer *link;
};

typedef struct SAudioSampleDescriptor
{
  unsigned int format;
  int sample_size;
  int sample_rate;
  int channel_count;
  unsigned int compression_type;
  int compression_ratio;
  int sample_count;
} SAudioSampleDescriptor;

typedef struct SAudioHeaderChunk
{
  int chunk_type;
  int chunk_size;
  int stream_time;
  int channel;
  int subchunk_type;
  int version;
  int buffer_count;
  int initial_amplitude;
  int initial_pan;
  SAudioSampleDescriptor sample;
} SAudioHeaderChunk;

typedef struct SAudioSampleChunk
{
  int chunk_type;
  int chunk_size;
  int stream_time;
  int channel;
  int subchunk_type;
  int sample_bytes;
  unsigned char samples[4];
} SAudioSampleChunk;

typedef union SAudioControlBlock
{
  struct
  {
    long *tag_list;
  } load_templates;
  struct
  {
    long channel;
    long value;
  } amplitude;
  struct
  {
    long channel;
    long value;
  } pan;
  struct
  {
    long channel;
  } mute;
  struct
  {
    long channel;
  } unmute;
  struct
  {
    long channel;
  } clock;
  struct
  {
    long channel;
  } close_channel;
  struct
  {
    long channel;
  } flush_channel;
} SAudioControlBlock;

typedef struct SAudioOutput
{
  Item instrument;
  long channel_count;
  Item left_envelope;
  Item left_envelope_target_knob;
  Item right_envelope;
  Item right_envelope_target_knob;
  long current_amplitude;
  long saved_amplitude;
  boolean muted;
  boolean external_mute;
  unsigned char alignment[2];
  long current_pan;
} SAudioOutput;

typedef struct SAudioChannel
{
  unsigned int status;
  int buffer_count;
  MemPoolPtr buffer_pool;
  Item instrument;
  unsigned char instrument_started;
  unsigned char attachments_running;
  unsigned char alignment[2];
  unsigned int signal_mask;
  SAudioMessageQueue pending_messages;
  int in_use_count;
  SAudioBuffer *in_use_queue_head;
  SAudioBuffer *in_use_queue_tail;
  SAudioOutput output;
} SAudioChannel;

typedef struct SAudioBufferInitBlock
{
  SAudioChannel *channel;
  SAudioHeaderChunk *header;
} SAudioBufferInitBlock;

typedef struct SAudioBufferInitState
{
  SAudioBufferInitBlock parameters;
  SAudioBuffer *initialized_head;
  int status;
} SAudioBufferInitState;

typedef union SAudioSubscriberMessagePayload
{
  struct
  {
    void *buffer;
  } data;
  struct
  {
    int number;
    int status;
  } channel;
  struct
  {
    int argument;
    void *data;
  } control;
  struct
  {
    unsigned int clock;
  } synchronize;
  struct
  {
    unsigned int options;
  } start;
  struct
  {
    unsigned int options;
  } stop;
} SAudioSubscriberMessagePayload;

struct SAudioSubscriberMessage
{
  /* The wire field is a 32-bit long; ARMCC may narrow enum storage. */
  int operation;
  Item message_item;
  void *private_data;
  void *link;
  SAudioSubscriberMessagePayload payload;
};

typedef struct SAudioContext
{
  Item creator_task;
  unsigned int creator_signal;
  int creator_status;
  Item thread_item;
  void *thread_stack_block;
  Item request_port;
  unsigned int request_port_signal;
  DSStreamCBPtr stream_control;
  SAudioTemplateRecord *template_array;
  unsigned int all_buffer_signals;
  Item output_template_item;
  Item envelope_template_item;
  int clock_channel;
  SAudioChannel channels[SAUDIO_SUBSCRIBER_CHANNEL_COUNT];
} SAudioContext;

typedef char SAudioOutputSizeCheck[
    sizeof(SAudioOutput) == SAUDIO_OUTPUT_SIZE ? 1 : -1];
typedef char SAudioBufferSizeCheck[
    sizeof(SAudioBuffer) == SAUDIO_BUFFER_SIZE ? 1 : -1];
typedef char SAudioBufferInitBlockSizeCheck[
    sizeof(SAudioBufferInitBlock) == SAUDIO_BUFFER_INIT_BLOCK_SIZE ? 1 : -1];
typedef char SAudioControlBlockSizeCheck[
    sizeof(SAudioControlBlock) == SAUDIO_CONTROL_BLOCK_SIZE ? 1 : -1];
typedef char SAudioHeaderChunkSizeCheck[
    sizeof(SAudioHeaderChunk) == SAUDIO_HEADER_CHUNK_SIZE ? 1 : -1];
typedef char SAudioSampleDescriptorSizeCheck[
    sizeof(SAudioSampleDescriptor) == SAUDIO_SAMPLE_DESCRIPTOR_SIZE ? 1 : -1];
typedef char SAudioSampleChunkSizeCheck[
    sizeof(SAudioSampleChunk) == SAUDIO_SAMPLE_CHUNK_SIZE ? 1 : -1];
typedef char SAudioChannelSizeCheck[
    sizeof(SAudioChannel) == SAUDIO_CHANNEL_SIZE ? 1 : -1];
typedef char SAudioOutputInstrumentOffsetCheck[
    offsetof(SAudioOutput, instrument) ==
    SAUDIO_OUTPUT_INSTRUMENT_OFFSET ? 1 : -1];
typedef char SAudioOutputChannelCountOffsetCheck[
    offsetof(SAudioOutput, channel_count) ==
    SAUDIO_OUTPUT_CHANNEL_COUNT_OFFSET ? 1 : -1];
typedef char SAudioOutputLeftEnvelopeOffsetCheck[
    offsetof(SAudioOutput, left_envelope) ==
    SAUDIO_OUTPUT_LEFT_ENVELOPE_OFFSET ? 1 : -1];
typedef char SAudioOutputLeftTargetKnobOffsetCheck[
    offsetof(SAudioOutput, left_envelope_target_knob) ==
    SAUDIO_OUTPUT_LEFT_TARGET_KNOB_OFFSET ? 1 : -1];
typedef char SAudioOutputRightEnvelopeOffsetCheck[
    offsetof(SAudioOutput, right_envelope) ==
    SAUDIO_OUTPUT_RIGHT_ENVELOPE_OFFSET ? 1 : -1];
typedef char SAudioOutputRightTargetKnobOffsetCheck[
    offsetof(SAudioOutput, right_envelope_target_knob) ==
    SAUDIO_OUTPUT_RIGHT_TARGET_KNOB_OFFSET ? 1 : -1];
typedef char SAudioOutputCurrentAmplitudeOffsetCheck[
    offsetof(SAudioOutput, current_amplitude) ==
    SAUDIO_OUTPUT_CURRENT_AMPLITUDE_OFFSET ? 1 : -1];
typedef char SAudioOutputSavedAmplitudeOffsetCheck[
    offsetof(SAudioOutput, saved_amplitude) ==
    SAUDIO_OUTPUT_SAVED_AMPLITUDE_OFFSET ? 1 : -1];
typedef char SAudioOutputMutedOffsetCheck[
    offsetof(SAudioOutput, muted) ==
    SAUDIO_OUTPUT_MUTED_OFFSET ? 1 : -1];
typedef char SAudioOutputExternalMuteOffsetCheck[
    offsetof(SAudioOutput, external_mute) ==
    SAUDIO_OUTPUT_EXTERNAL_MUTE_OFFSET ? 1 : -1];
typedef char SAudioOutputCurrentPanOffsetCheck[
    offsetof(SAudioOutput, current_pan) ==
    SAUDIO_OUTPUT_CURRENT_PAN_OFFSET ? 1 : -1];
typedef char SAudioChannelStatusOffsetCheck[
    offsetof(SAudioChannel, status) ==
    SAUDIO_CHANNEL_STATUS_OFFSET ? 1 : -1];
typedef char SAudioChannelBufferCountOffsetCheck[
    offsetof(SAudioChannel, buffer_count) ==
    SAUDIO_CHANNEL_BUFFER_COUNT_OFFSET ? 1 : -1];
typedef char SAudioChannelBufferPoolOffsetCheck[
    offsetof(SAudioChannel, buffer_pool) ==
    SAUDIO_CHANNEL_BUFFER_POOL_OFFSET ? 1 : -1];
typedef char SAudioChannelInstrumentOffsetCheck[
    offsetof(SAudioChannel, instrument) ==
    SAUDIO_CHANNEL_INSTRUMENT_OFFSET ? 1 : -1];
typedef char SAudioChannelInstrumentStartedOffsetCheck[
    offsetof(SAudioChannel, instrument_started) ==
    SAUDIO_CHANNEL_INSTRUMENT_STARTED_OFFSET ? 1 : -1];
typedef char SAudioChannelAttachmentsRunningOffsetCheck[
    offsetof(SAudioChannel, attachments_running) ==
    SAUDIO_CHANNEL_ATTACHMENTS_RUNNING_OFFSET ? 1 : -1];
typedef char SAudioChannelSignalMaskOffsetCheck[
    offsetof(SAudioChannel, signal_mask) ==
    SAUDIO_CHANNEL_SIGNAL_MASK_OFFSET ? 1 : -1];
typedef char SAudioChannelPendingMessagesOffsetCheck[
    offsetof(SAudioChannel, pending_messages) ==
    SAUDIO_CHANNEL_PENDING_MESSAGES_OFFSET ? 1 : -1];
typedef char SAudioChannelInUseCountOffsetCheck[
    offsetof(SAudioChannel, in_use_count) ==
    SAUDIO_CHANNEL_IN_USE_COUNT_OFFSET ? 1 : -1];
typedef char SAudioChannelInUseQueueHeadOffsetCheck[
    offsetof(SAudioChannel, in_use_queue_head) ==
    SAUDIO_CHANNEL_IN_USE_QUEUE_HEAD_OFFSET ? 1 : -1];
typedef char SAudioChannelInUseQueueTailOffsetCheck[
    offsetof(SAudioChannel, in_use_queue_tail) ==
    SAUDIO_CHANNEL_IN_USE_QUEUE_TAIL_OFFSET ? 1 : -1];
typedef char SAudioChannelOutputOffsetCheck[
    offsetof(SAudioChannel, output) ==
    SAUDIO_CHANNEL_OUTPUT_OFFSET ? 1 : -1];
typedef char SAudioMessageSizeCheck[
    sizeof(SAudioSubscriberMessage) == SAUDIO_MESSAGE_SIZE ? 1 : -1];
typedef char SAudioMessageItemOffsetCheck[
    offsetof(SAudioSubscriberMessage, message_item) ==
    SAUDIO_MESSAGE_ITEM_OFFSET ? 1 : -1];
typedef char SAudioMessageLinkOffsetCheck[
    offsetof(SAudioSubscriberMessage, link) ==
    SAUDIO_MESSAGE_LINK_OFFSET ? 1 : -1];
typedef char SAudioMessagePayloadOffsetCheck[
    offsetof(SAudioSubscriberMessage, payload) ==
    SAUDIO_MESSAGE_PAYLOAD_OFFSET ? 1 : -1];
typedef char SAudioMessageQueueHeadOffsetCheck[
    offsetof(SAudioMessageQueue, head) ==
    SAUDIO_MESSAGE_QUEUE_HEAD_OFFSET ? 1 : -1];
typedef char SAudioMessageQueueTailOffsetCheck[
    offsetof(SAudioMessageQueue, tail) ==
    SAUDIO_MESSAGE_QUEUE_TAIL_OFFSET ? 1 : -1];
typedef char SAudioBufferSampleOffsetCheck[
    offsetof(SAudioBuffer, sample) == SAUDIO_BUFFER_SAMPLE_OFFSET ? 1 : -1];
typedef char SAudioBufferAttachmentOffsetCheck[
    offsetof(SAudioBuffer, attachment) ==
    SAUDIO_BUFFER_ATTACHMENT_OFFSET ? 1 : -1];
typedef char SAudioBufferCueOffsetCheck[
    offsetof(SAudioBuffer, cue) == SAUDIO_BUFFER_CUE_OFFSET ? 1 : -1];
typedef char SAudioBufferSignalOffsetCheck[
    offsetof(SAudioBuffer, signal) == SAUDIO_BUFFER_SIGNAL_OFFSET ? 1 : -1];
typedef char SAudioBufferPendingMessageOffsetCheck[
    offsetof(SAudioBuffer, pending_message) ==
    SAUDIO_BUFFER_PENDING_MESSAGE_OFFSET ? 1 : -1];
typedef char SAudioBufferLinkOffsetCheck[
    offsetof(SAudioBuffer, link) == SAUDIO_BUFFER_LINK_OFFSET ? 1 : -1];
typedef char SAudioBufferInitChannelOffsetCheck[
    offsetof(SAudioBufferInitBlock, channel) ==
    SAUDIO_BUFFER_INIT_CHANNEL_OFFSET ? 1 : -1];
typedef char SAudioBufferInitHeaderOffsetCheck[
    offsetof(SAudioBufferInitBlock, header) ==
    SAUDIO_BUFFER_INIT_HEADER_OFFSET ? 1 : -1];
typedef char SAudioHeaderChannelOffsetCheck[
    offsetof(SAudioHeaderChunk, channel) ==
    SAUDIO_HEADER_CHANNEL_OFFSET ? 1 : -1];
typedef char SAudioHeaderVersionOffsetCheck[
    offsetof(SAudioHeaderChunk, version) ==
    SAUDIO_HEADER_VERSION_OFFSET ? 1 : -1];
typedef char SAudioHeaderBufferCountOffsetCheck[
    offsetof(SAudioHeaderChunk, buffer_count) ==
    SAUDIO_HEADER_BUFFER_COUNT_OFFSET ? 1 : -1];
typedef char SAudioHeaderInitialAmplitudeOffsetCheck[
    offsetof(SAudioHeaderChunk, initial_amplitude) ==
    SAUDIO_HEADER_INITIAL_AMPLITUDE_OFFSET ? 1 : -1];
typedef char SAudioHeaderInitialPanOffsetCheck[
    offsetof(SAudioHeaderChunk, initial_pan) ==
    SAUDIO_HEADER_INITIAL_PAN_OFFSET ? 1 : -1];
typedef char SAudioHeaderSampleDescriptorOffsetCheck[
    offsetof(SAudioHeaderChunk, sample) ==
    SAUDIO_HEADER_SAMPLE_DESCRIPTOR_OFFSET ? 1 : -1];
typedef char SAudioSampleDescriptorChannelCountOffsetCheck[
    offsetof(SAudioSampleDescriptor, channel_count) ==
    SAUDIO_SAMPLE_DESCRIPTOR_CHANNEL_COUNT_OFFSET ? 1 : -1];
typedef char SAudioSampleChunkChannelOffsetCheck[
    offsetof(SAudioSampleChunk, channel) ==
    SAUDIO_SAMPLE_CHUNK_CHANNEL_OFFSET ? 1 : -1];
typedef char SAudioSampleChunkStreamTimeOffsetCheck[
    offsetof(SAudioSampleChunk, stream_time) ==
    SAUDIO_SAMPLE_CHUNK_STREAM_TIME_OFFSET ? 1 : -1];
typedef char SAudioSampleChunkSampleBytesOffsetCheck[
    offsetof(SAudioSampleChunk, sample_bytes) ==
    SAUDIO_SAMPLE_CHUNK_SAMPLE_BYTES_OFFSET ? 1 : -1];
typedef char SAudioSampleChunkSamplesOffsetCheck[
    offsetof(SAudioSampleChunk, samples) ==
    SAUDIO_SAMPLE_CHUNK_SAMPLES_OFFSET ? 1 : -1];
typedef char SAudioContextSizeCheck[
    sizeof(SAudioContext) == SAUDIO_CONTEXT_SIZE ? 1 : -1];
typedef char SAudioContextStreamControlOffsetCheck[
    offsetof(SAudioContext, stream_control) ==
    SAUDIO_CONTEXT_STREAM_CONTROL_OFFSET ? 1 : -1];
typedef char SAudioContextTemplateArrayOffsetCheck[
    offsetof(SAudioContext, template_array) ==
    SAUDIO_CONTEXT_TEMPLATE_ARRAY_OFFSET ? 1 : -1];
typedef char SAudioContextBufferSignalsOffsetCheck[
    offsetof(SAudioContext, all_buffer_signals) ==
    SAUDIO_CONTEXT_BUFFER_SIGNALS_OFFSET ? 1 : -1];
typedef char SAudioContextOutputTemplateOffsetCheck[
    offsetof(SAudioContext, output_template_item) ==
    SAUDIO_CONTEXT_OUTPUT_TEMPLATE_OFFSET ? 1 : -1];
typedef char SAudioContextEnvelopeTemplateOffsetCheck[
    offsetof(SAudioContext, envelope_template_item) ==
    SAUDIO_CONTEXT_ENVELOPE_TEMPLATE_OFFSET ? 1 : -1];
typedef char SAudioContextClockChannelOffsetCheck[
    offsetof(SAudioContext, clock_channel) ==
    SAUDIO_CONTEXT_CLOCK_CHANNEL_OFFSET ? 1 : -1];
typedef char SAudioContextChannelsOffsetCheck[
    offsetof(SAudioContext, channels) ==
    SAUDIO_CONTEXT_CHANNELS_OFFSET ? 1 : -1];
typedef char SAudioMemoryPoolSizeCheck[
    sizeof(MemPool) == SAUDIO_MEMORY_POOL_SIZE ? 1 : -1];

int
create_data_stream_thread(void  *thread_entry,
                          int    stack_size,
                          int    priority,
                          char  *name,
                          void **stack_block,
                          int    initial_r0,
                          void  *initial_r1);
int
dispose_data_stream_thread(Item thread_item);

int
initialize_saudio_subscriber_context_pool(void);
int
dispose_saudio_subscriber_context_pool(void);
int
create_saudio_subscriber_context(SAudioContext **created_context,
                                 DSStreamCBPtr   stream_control,
                                 int             priority_delta);
int
dispose_saudio_subscriber_context(SAudioContext *context);

void
saudio_subscriber_thread_main(int            ignored_argument,
                              SAudioContext *context);
int
initialize_saudio_subscriber_thread(SAudioContext *context);
int
process_saudio_data_message(SAudioContext           *context,
                            SAudioSubscriberMessage *message);
int
set_saudio_channel_subscription(SAudioContext           *context,
                                SAudioSubscriberMessage *message);
int
get_saudio_channel_status(SAudioContext           *context,
                          SAudioSubscriberMessage *message);
int
dispatch_saudio_control_command(SAudioContext           *context,
                                SAudioSubscriberMessage *message);
int
acknowledge_saudio_stream_open(void);
int
close_saudio_stream_channels(SAudioContext *context);
int
start_saudio_stream_channels(SAudioContext           *context,
                             SAudioSubscriberMessage *message);
int
stop_saudio_stream_channels(SAudioContext           *context,
                            SAudioSubscriberMessage *message);
int
synchronize_saudio_stream_channels(SAudioContext           *context,
                                   SAudioSubscriberMessage *message);
int
acknowledge_saudio_end_of_file(void);
int
abort_saudio_stream(SAudioContext *context);

boolean
InitAudioBuffer(void *visitor_context,
                void *pool_entry);
boolean
FreeAudioBuffer(void *visitor_context,
                void *pool_entry);
int
initialize_saudio_buffer_pool(SAudioChannel     *channel,
                              SAudioHeaderChunk *header);
boolean
AddBufferToTail(SAudioChannel *channel,
                SAudioBuffer  *buffer);
SAudioBuffer *
GetNextBuffer(SAudioChannel *channel);
void
FreeBufferFromAudioFolio(SAudioBuffer *buffer);
void
OrphanPlayingBuffers(SAudioContext *context);
void
MoveWaitingMsgsToBufferQueue(SAudioContext *context,
                             int            channel_number);
void
QueueNewAudioBuffer(SAudioContext           *context,
                    SAudioSubscriberMessage *message);
void
FindBuffer(SAudioContext  *context,
           uint32          signal_bits,
           SAudioChannel **channel_out,
           SAudioBuffer  **buffer_out);
void
HandleCompletedBuffers(SAudioContext *context,
                       uint32         signal_bits);

int
begin_saudio_playback_if_ready(SAudioContext *context,
                               int            channel_number);
int
InitChannel(SAudioContext     *context,
            SAudioHeaderChunk *header);
int
StartChannel(SAudioContext *context,
             int            channel_number);
long
StopChannel(SAudioContext *context,
            long           channel_number);
long
FlushChannel(SAudioContext *context,
             long           channel_number);
long
CloseChannel(SAudioContext *context,
             long           channel_number);
long
SetChannelAmplitude(SAudioContext *context,
                    long           channel_number,
                    long           new_amplitude);
long
SetChannelPan(SAudioContext *context,
              long           channel_number,
              long           new_pan);
long
GetChannelAmplitude(SAudioContext *context,
                    long           channel_number,
                    long          *amplitude);
long
GetChannelPan(SAudioContext *context,
              long           channel_number,
              long          *pan);
long
MuteChannel(SAudioContext *context,
            long           channel_number,
            boolean        external);
long
UnMuteChannel(SAudioContext *context,
              long           channel_number,
              boolean        external);
long
get_legacy_saudio_template_tag(SAudioSampleDescriptor *descriptor);
Item
get_legacy_saudio_template_item(SAudioTemplateRecord *templates,
                                long                  template_tag,
                                long                  template_count);
long
LoadTemplates(SAudioTemplateRecord *templates,
              long                 *template_tags,
              long                  template_count);

#endif

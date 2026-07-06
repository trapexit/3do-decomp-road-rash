#include "stdio.h"
#include "stdlib.h"

#include "audio.h"
#include "datastreamdebug.h"
#include "sdk_audio_compat.h"

#include "audio_message_runtime.h"
#include "control_subscriber_runtime.h"
#include "data_stream_runtime.h"
#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

#define CONTROL_DS_REQUEST_MESSAGE_BYTES 0x1C
#define CONTROL_SUBSCRIBER_MESSAGE_BYTES 0x18
#define CONTROL_SUBCHUNK_TYPE_COUNT 3
#define CONTROL_SUBCHUNK_TYPE_TABLE_BYTES 0x0C

#define CONTROL_CUES_SUBTYPE_INDEX 0
#define CONTROL_GOTO_SUBTYPE_INDEX 1
#define CONTROL_SYNC_SUBTYPE_INDEX 2

#define CHECK_CONTROL_STREAM_RESULT(operation_name, operation_status)       \
        if((int32)(operation_status) < 0) {                                    \
            printf(control_failure_format, operation_name,                      \
                   (int32)(operation_status));                                  \
            PrintfDSError((int32)(operation_status));                            \
            return (int32)(operation_status);                                    \
          }

typedef char ControlDSRequestMessageSizeCheck[
    sizeof(DSRequestMsg) == CONTROL_DS_REQUEST_MESSAGE_BYTES ? 1 : -1];
typedef char ControlSubscriberMessageSizeCheck[
    sizeof(SubscriberMsg) == CONTROL_SUBSCRIBER_MESSAGE_BYTES ? 1 : -1];
typedef char ControlSubchunkTypeTableSizeCheck[
    sizeof(int32) * CONTROL_SUBCHUNK_TYPE_COUNT ==
    CONTROL_SUBCHUNK_TYPE_TABLE_BYTES
        ? 1 : -1];

static const int32 control_subchunk_types[CONTROL_SUBCHUNK_TYPE_COUNT] =
{
  CONTROL_CUES_CHUNK_SUBTYPE,
  GOTO_CHUNK_SUBTYPE,
  SYNC_CHUNK_SUBTYPE
};
static const char control_marker_check_operation[36] =
  "ControlSubscriber - DSIsMarker() = ";
static const char control_failure_format[24] =
  "Failure in %s: $%lx\n";
static const char control_clock_sync_operation[40] =
  "ControlSubscriber - DSClockSync() = ";
static const char control_go_marker_operation[36] =
  "ControlSubscriber - DSGoMarker() = ";

static
void
stop_control_channel(RoadRashControlContext *context,
                     long                    channel_number)
{
  SubsChannelPtr channel;

  if(channel_number < CTRL_MAX_CHANNELS)
    {
      channel = &context->standard.channel[channel_number];
      channel->status &= ~CHAN_ENABLED;
    }
}


static
void
flush_control_channel(RoadRashControlContext *context,
                      long                    channel_number)
{
  SubsChannelPtr channel;
  SubscriberMsgPtr message;
  SubscriberMsgPtr next_message;

  if(channel_number < CTRL_MAX_CHANNELS)
    {
      channel = &context->standard.channel[channel_number];
      stop_control_channel(context, channel_number);

      while((message = channel->msgQueue.head) != 0)
        {
          next_message = (SubscriberMsgPtr)message->link;
          ReplyMsg(message->msgItem, kDSNoErr, message,
                   sizeof(SubscriberMsgPtr));
          channel->msgQueue.head = next_message;
        }
      channel->msgQueue.tail = 0;
    }
}


int32
process_control_goto_chunk(ControlChunkPtr         control_chunk,
                           RoadRashControlContext *context);
int32
process_control_cues_chunk(ControlChunkPtr         control_chunk,
                           RoadRashControlContext *context);

static
int32
process_control_data_chunk(SubscriberMsgPtr        message,
                           RoadRashControlContext *context)
{
  ControlChunkPtr control_chunk;
  boolean at_marker_position;
  int32 status;

  control_chunk = (ControlChunkPtr)message->msg.data.buffer;

  if(control_chunk->subChunkType ==
     control_subchunk_types[CONTROL_SYNC_SUBTYPE_INDEX])
    {
      status = road_rash_ds_is_marker(context->standard.streamCBPtr,
                                      control_chunk->u.sync.value,
                                      &at_marker_position);
      CHECK_CONTROL_STREAM_RESULT(control_marker_check_operation, status);
      if(status != kDSNoErr || at_marker_position == false)
        {
          return status;
        }

      status = DSSetClock(context->standard.streamCBPtr,
                          control_chunk->time);
      if(status != kDSNoErr)
        {
          return status;
        }

      status = synchronize_legacy_data_stream_clock(context->standard.streamCBPtr,
                                                    context->standard.subsMsgPool,
                                                    control_chunk->time);
      CHECK_CONTROL_STREAM_RESULT(control_clock_sync_operation, status);
      return status;
    }

  if(control_chunk->subChunkType ==
     control_subchunk_types[CONTROL_GOTO_SUBTYPE_INDEX])
    {
      return process_control_goto_chunk(control_chunk, context);
    }

  if(control_chunk->subChunkType ==
     control_subchunk_types[CONTROL_CUES_SUBTYPE_INDEX])
    {
      return process_control_cues_chunk(control_chunk, context);
    }
  return kDSNoErr;
}


int32
process_control_goto_chunk(ControlChunkPtr         control_chunk,
                           RoadRashControlContext *context)
{
  DSRequestMsgPtr request;
  int32 status;

  request = (DSRequestMsgPtr)AllocPoolMem(
    context->standard.dsReqMsgPool);
  if(request == 0)
    {
      status = CONTROL_SUBSCRIBER_LEGACY_NO_MESSAGE_ERROR;
    }
  else
    {
      status = seek_legacy_data_stream_marker(request->msgItem, request,
                                              context->standard.streamCBPtr,
                                              control_chunk->u.marker.value,
                                              GOMARKER_ABSOLUTE);
      if(status >= kDSNoErr)
        {
          return status;
        }
      ReturnPoolMem(context->standard.dsReqMsgPool, request);
    }
  CHECK_CONTROL_STREAM_RESULT(control_go_marker_operation, status);
  return status;
}


int32
process_control_cues_chunk(ControlChunkPtr         control_chunk,
                           RoadRashControlContext *context)
{
  RoadRashControlCueChunk *cue_chunk;

  cue_chunk = (RoadRashControlCueChunk *)control_chunk;
  if(cue_chunk->version == CONTROL_CUES_CHUNK_VERSION)
    {
      load_control_cue_anchors(
        context, cue_chunk->time, cue_chunk->cue_anchor_count,
        cue_chunk->cue_anchors);
    }
  return kDSNoErr;
}


static
int32
get_control_channel_status(SubscriberMsgPtr        message,
                           RoadRashControlContext *context)
{
  long channel_number;

  channel_number = message->msg.channel.number;
  if(channel_number < CTRL_MAX_CHANNELS)
    {
      return context->standard.channel[channel_number].status;
    }
  return 0;
}


static
int32
set_control_channel_subscription(SubscriberMsgPtr        message,
                                 RoadRashControlContext *context)
{
  SubsChannelPtr channel;
  long channel_number;

  channel_number = message->msg.channel.number;
  if(channel_number < CTRL_MAX_CHANNELS)
    {
      channel = &context->standard.channel[channel_number];
      channel->status |=
        ~CHAN_SYSBITS & message->msg.channel.status;
    }
  return kDSNoErr;
}


static
int32
handle_control_message(SubscriberMsgPtr        message,
                       RoadRashControlContext *context)
{
  (void)message;
  (void)context;
  return kDSNoErr;
}


static
int32
acknowledge_control_stream_opening(SubscriberMsgPtr        message,
                                   RoadRashControlContext *context)
{
  (void)message;
  (void)context;
  return kDSNoErr;
}


static
int32
acknowledge_control_stream_closing(SubscriberMsgPtr        message,
                                   RoadRashControlContext *context)
{
  (void)message;
  (void)context;
  return kDSNoErr;
}


static
int32
stop_control_subscription(SubscriberMsgPtr        message,
                          RoadRashControlContext *context)
{
  long channel_number;

  (void)message;
  for(channel_number = 0;
      channel_number < CTRL_MAX_CHANNELS;
      channel_number++)
    {
      stop_control_channel(context, channel_number);
    }
  return kDSNoErr;
}


static
int32
synchronize_control_subscription(SubscriberMsgPtr        message,
                                 RoadRashControlContext *context)
{
  (void)message;
  (void)context;
  return kDSNoErr;
}


static
int32
acknowledge_control_end_of_file(SubscriberMsgPtr        message,
                                RoadRashControlContext *context)
{
  (void)message;
  (void)context;
  return kDSNoErr;
}


static
int32
abort_control_subscription(SubscriberMsgPtr        message,
                           RoadRashControlContext *context)
{
  long channel_number;

  (void)message;
  for(channel_number = 0;
      channel_number < CTRL_MAX_CHANNELS;
      channel_number++)
    {
      flush_control_channel(context, channel_number);
    }
  return kDSNoErr;
}


int32
initialize_control_subscriber_thread(RoadRashControlContext *context)
{
  CtrlContextPtr standard_context;
  int32 status;
  long channel_number;

  standard_context = &context->standard;
  standard_context->creatorStatus = -1;
  standard_context->requestPort = 0;
  standard_context->cueItem = 0;
  standard_context->cueSignal = 0;
  standard_context->fTimerRunning = false;

  status = OpenAudioFolio();
  if(status < 0)
    {
      standard_context->creatorStatus = status;
      goto signal_creator;
    }

  standard_context->subsReplyPortSignal = 0;
  standard_context->subsReplyPort = 0;
  standard_context->subsMsgPool = 0;
  standard_context->dsReqReplyPort = 0;
  standard_context->dsReqReplyPortSignal = 0;
  standard_context->dsReqMsgPool = 0;

  for(channel_number = 0;
      channel_number < CTRL_MAX_CHANNELS;
      channel_number++)
    {
      standard_context->channel[channel_number].status = CHAN_ENABLED;
      standard_context->channel[channel_number].msgQueue.head = 0;
      standard_context->channel[channel_number].msgQueue.tail = 0;
    }
  context->cue_anchor_count = 0;

  status = NewMsgPort(&standard_context->requestPortSignal);
  if(status <= 0)
    {
      goto signal_creator;
    }
  standard_context->requestPort = status;

  status = NewMsgPort(&standard_context->dsReqReplyPortSignal);
  if(status <= 0)
    {
      goto signal_creator;
    }
  standard_context->dsReqReplyPort = status;

  standard_context->creatorStatus =
    CONTROL_SUBSCRIBER_LEGACY_NO_MEMORY_ERROR;
  standard_context->dsReqMsgPool = (MemPoolPtr)create_memory_pool(
    CTRL_NUM_DS_REQS_MSGS, sizeof(DSRequestMsg));
  if(standard_context->dsReqMsgPool == 0)
    {
      goto signal_creator;
    }
  if(FillPoolWithMsgItems(standard_context->dsReqMsgPool,
                          standard_context->dsReqReplyPort) == false)
    {
      goto signal_creator;
    }

  status = NewMsgPort(&standard_context->subsReplyPortSignal);
  if(status <= 0)
    {
      goto signal_creator;
    }
  standard_context->subsReplyPort = status;

  standard_context->subsMsgPool = (MemPoolPtr)create_memory_pool(
    CTRL_MAX_SUBS_MESSAGES, sizeof(SubscriberMsg));
  if(standard_context->subsMsgPool == 0)
    {
      goto signal_creator;
    }
  if(FillPoolWithMsgItems(standard_context->subsMsgPool,
                          standard_context->subsReplyPort) == false)
    {
      goto signal_creator;
    }

  status = CreateSizedItem(
    MKNODEID(AUDIONODE, AUDIO_CUE_NODE), 0, 0);
  if(status <= 0)
    {
      goto signal_creator;
    }
  standard_context->cueItem = status;
  standard_context->cueSignal = GetCueSignal(standard_context->cueItem);
  if(standard_context->cueSignal != 0)
    {
      standard_context->creatorStatus = kDSNoErr;
    }

signal_creator:
  status = SendSignal(standard_context->creatorTask,
                      standard_context->creatorSignal);
  if(standard_context->creatorStatus < 0 || status < 0)
    {
      return -1;
    }
  return kDSNoErr;
}


void
control_subscriber_thread_main(int32                   ignored_initial_argument,
                               RoadRashControlContext *context)
{
  CtrlContextPtr standard_context;
  SubscriberMsgPtr message;
  SubscriberMsgPtr subscriber_reply;
  DSRequestMsgPtr stream_request_reply;
  SubscriberMsgPtr closing_message;
  uint32 awaited_signals;
  uint32 received_signals;
  int32 status;
  int keep_running;
  long channel_number;

  (void)ignored_initial_argument;
  standard_context = &context->standard;

  status = initialize_control_subscriber_thread(context);
  if(status != kDSNoErr)
    {
      exit(0);
    }

  awaited_signals = standard_context->requestPortSignal |
                    standard_context->cueSignal |
                    standard_context->subsReplyPortSignal |
                    standard_context->dsReqReplyPortSignal;

  keep_running = 1;
  closing_message = 0;
  while(keep_running != 0 ||
        standard_context->dsReqMsgPool->numFreeInPool !=
          standard_context->dsReqMsgPool->numItemsInPool ||
        standard_context->subsMsgPool->numFreeInPool !=
          standard_context->subsMsgPool->numItemsInPool)
    {
      received_signals = WaitSignal(awaited_signals);

      /* Road Rash retains the stock cue signal but schedules no timer. */
      if((received_signals &
          standard_context->subsReplyPortSignal) != 0)
        {
          while(PollForMsg(standard_context->subsReplyPort,
                           0, 0, (void **)&subscriber_reply,
                           &status) != 0)
            {
              ReturnPoolMem(standard_context->subsMsgPool,
                            subscriber_reply);
            }
        }

      if((received_signals &
          standard_context->dsReqReplyPortSignal) != 0)
        {
          while(PollForMsg(standard_context->dsReqReplyPort,
                           0, 0, (void **)&stream_request_reply,
                           &status) != 0)
            {
              ReturnPoolMem(standard_context->dsReqMsgPool,
                            stream_request_reply);
            }
        }

      if((received_signals & standard_context->requestPortSignal) != 0)
        {
          while(PollForMsg(standard_context->requestPort,
                           0, 0, (void **)&message, &status) != 0)
            {
              switch(message->whatToDo)
                {
                case kStreamOpData:
                  status = process_control_data_chunk(
                    message, context);
                  break;
                case kStreamOpGetChan:
                  status = get_control_channel_status(message, context);
                  break;
                case kStreamOpSetChan:
                  status = set_control_channel_subscription(
                    message, context);
                  break;
                case kStreamOpControl:
                  status = handle_control_message(
                    message, context);
                  break;
                case kStreamOpSync:
                  status = synchronize_control_subscription(
                    message, context);
                  break;
                case kStreamOpStart:
                  break;
                case kStreamOpStop:
                  status = stop_control_subscription(message, context);
                  break;
                case kStreamOpOpening:
                  status = acknowledge_control_stream_opening(
                    message, context);
                  break;
                case kStreamOpClosing:
                  status = acknowledge_control_stream_closing(
                    message, context);
                  keep_running = 0;
                  message->link = closing_message;
                  closing_message = message;
                  break;
                case kStreamOpEOF:
                  status = acknowledge_control_end_of_file(
                    message, context);
                  break;
                case kStreamOpAbort:
                  status = abort_control_subscription(message, context);
                  keep_running = 0;
                  message->link = closing_message;
                  closing_message = message;
                  break;
                default:
                  break;
                }

              if(message != closing_message)
                {
                  ReplyMsg(message->msgItem, status, message,
                           sizeof(SubscriberMsg));
                }
            }
        }
    }

  for(channel_number = 0;
      channel_number < CTRL_MAX_CHANNELS;
      channel_number++)
    {
      flush_control_channel(context, channel_number);
    }
  while(closing_message != 0)
    {
      message = closing_message;
      closing_message = (SubscriberMsgPtr)message->link;
      ReplyMsg(message->msgItem, kDSNoErr, message,
               sizeof(SubscriberMsg));
    }
  exit(0);
}

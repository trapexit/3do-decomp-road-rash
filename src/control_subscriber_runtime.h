#ifndef CONTROL_SUBSCRIBER_RUNTIME_H
#define CONTROL_SUBSCRIBER_RUNTIME_H

#ifndef offsetof
  #include "stddef.h"
#endif

#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "controlsubscriber.h"

#define CONTROL_SUBSCRIBER_CONTEXT_BYTES      0xE0
#define CONTROL_SUBSCRIBER_THREAD_STACK_BYTES 0x1000
#define CONTROL_CUE_ANCHOR_CAPACITY            8
#define CONTROL_CUE_ANCHOR_BYTES               0x10
#define CONTROL_CUE_REQUEST_BYTES              0x18

#define CONTROL_CUES_CHUNK_SUBTYPE \
        CHAR4LITERAL('C', 'U', 'E', 'S')
#define CONTROL_CUES_CHUNK_VERSION 1

#define CONTROL_SUBSCRIBER_LEGACY_NO_MEMORY_ERROR (-0x3E0)
#define CONTROL_SUBSCRIBER_LEGACY_NO_SIGNAL_ERROR (-0x3DF)
#define CONTROL_SUBSCRIBER_LEGACY_SIGNAL_ERROR    (-0x3DE)
#define CONTROL_SUBSCRIBER_LEGACY_NO_MESSAGE_ERROR (-0x3E2)

typedef struct RoadRashControlCueAnchor
{
  int32 chunk_type;
  int32 subchunk_type;
  int32 base_time_for_primary_selector;
  int32 base_time_for_alternate_selector;
} RoadRashControlCueAnchor;

typedef void (*RoadRashControlCueCallback)(void *callback_argument,
                                           int32 relative_stream_time);

typedef struct RoadRashControlCueRequest
{
  int32 chunk_type;
  int32 subchunk_type;
  int32 use_alternate_base_time;
  int32 trigger_time_offset;
  RoadRashControlCueCallback callback;
  void *callback_argument;
} RoadRashControlCueRequest;

typedef struct RoadRashControlCueChunk
{
  SUBS_CHUNK_COMMON;
  int32 version;
  int32 cue_anchor_count;
  RoadRashControlCueAnchor cue_anchors[1];
} RoadRashControlCueChunk;

typedef struct RoadRashControlContext
{
  CtrlContext standard;
  int32 cue_anchor_count;
  RoadRashControlCueAnchor cue_anchors[CONTROL_CUE_ANCHOR_CAPACITY];
} RoadRashControlContext;

typedef char ControlContextBaseSizeCheck[
    sizeof(CtrlContext) == 0x5C ? 1 : -1];
typedef char ControlContextStreamOffsetCheck[
    offsetof(CtrlContext, streamCBPtr) == 0x0C ? 1 : -1];
typedef char ControlContextRequestPortOffsetCheck[
    offsetof(CtrlContext, requestPort) == 0x18 ? 1 : -1];
typedef char ControlContextChannelOffsetCheck[
    offsetof(CtrlContext, channel) == 0x50 ? 1 : -1];
typedef char ControlCueAnchorSizeCheck[
    sizeof(RoadRashControlCueAnchor) == CONTROL_CUE_ANCHOR_BYTES ? 1 : -1];
typedef char ControlCueRequestSizeCheck[
    sizeof(RoadRashControlCueRequest) == CONTROL_CUE_REQUEST_BYTES ? 1 : -1];
typedef char ControlCueRequestCallbackOffsetCheck[
    offsetof(RoadRashControlCueRequest, callback) == 0x10 ? 1 : -1];
typedef char ControlCueChunkVersionOffsetCheck[
    offsetof(RoadRashControlCueChunk, version) == 0x14 ? 1 : -1];
typedef char ControlCueChunkCountOffsetCheck[
    offsetof(RoadRashControlCueChunk, cue_anchor_count) == 0x18 ? 1 : -1];
typedef char ControlCueChunkAnchorsOffsetCheck[
    offsetof(RoadRashControlCueChunk, cue_anchors) == 0x1C ? 1 : -1];
typedef char ControlCueCountOffsetCheck[
    offsetof(RoadRashControlContext, cue_anchor_count) == 0x5C ? 1 : -1];
typedef char ControlCueTableOffsetCheck[
    offsetof(RoadRashControlContext, cue_anchors) == 0x60 ? 1 : -1];
typedef char ControlSubscriberContextSizeCheck[
    sizeof(RoadRashControlContext) == CONTROL_SUBSCRIBER_CONTEXT_BYTES
        ? 1 : -1];

int32
initialize_control_subscriber_pool(void);
int32
close_control_subscriber_pool(void);
int32
create_control_subscriber(RoadRashControlContext **created_context,
                          DSStreamCBPtr            stream,
                          int32                    priority_delta);
int32
dispose_control_subscriber(RoadRashControlContext *context);

void
load_control_cue_anchors(RoadRashControlContext         *context,
                         int32                           chunk_time,
                         int32                           cue_anchor_count,
                         const RoadRashControlCueAnchor *cue_anchors);
void
dispatch_due_control_cues(RoadRashControlContext    *context,
                          int32                      cue_request_count,
                          RoadRashControlCueRequest *cue_requests);

int32
initialize_control_subscriber_thread(RoadRashControlContext *context);
void
control_subscriber_thread_main(int32                   ignored_initial_argument,
                               RoadRashControlContext *context);

#endif

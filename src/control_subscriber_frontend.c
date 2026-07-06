#include "platform.h"
#include "string.h"

#include "control_subscriber_runtime.h"
#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"
#include "saudio_subscriber_runtime.h"

int
dispose_data_stream_thread_stack(void *stack_block);

int32
initialize_control_subscriber_pool(void)
{
  ctrl_subscriber_context_pool = create_memory_pool(
    CTRL_MAX_SUBSCRIPTIONS, CONTROL_SUBSCRIBER_CONTEXT_BYTES);
  if(ctrl_subscriber_context_pool == 0)
    {
      return -1;
    }
  return 0;
}


int32
close_control_subscriber_pool(void)
{
  destroy_memory_pool(ctrl_subscriber_context_pool);
  ctrl_subscriber_context_pool = 0;
  return 0;
}


int32
create_control_subscriber(RoadRashControlContext **created_context,
                          DSStreamCBPtr            stream,
                          int32                    priority_delta)
{
  MemPoolPtr context_pool;
  RoadRashControlContext *context;
  int32 status;

  context_pool = (MemPoolPtr)ctrl_subscriber_context_pool;
  context = (RoadRashControlContext *)AllocPoolMem(
    context_pool);
  if(context == 0)
    {
      return CONTROL_SUBSCRIBER_LEGACY_NO_MEMORY_ERROR;
    }

  memset(context, 0, sizeof(*context));
  context->standard.creatorTask = CURRENTTASK->t.n_Item;
  context->standard.streamCBPtr = stream;
  context->standard.creatorSignal = AllocSignal(0);
  if((int32)context->standard.creatorSignal <= 0)
    {
      status = CONTROL_SUBSCRIBER_LEGACY_NO_SIGNAL_ERROR;
      context->standard.creatorSignal = 0;
    }
  else
    {
      status = create_data_stream_thread(
        (void *)control_subscriber_thread_main,
        CONTROL_SUBSCRIBER_THREAD_STACK_BYTES,
        (int)CURRENTTASK->t.n_Priority + priority_delta, 0,
        &context->standard.threadStackBlock, 0, context);
      if(status > 0)
        {
          context->standard.threadItem = status;
          status = wait_for_data_stream_thread_startup(
            context->standard.threadItem, context->standard.creatorSignal);
          if(status < 0)
            {
              goto cleanup;
            }

          FreeSignal(context->standard.creatorSignal);
          context->standard.creatorSignal = 0;
          status = context->standard.creatorStatus;
          if(status >= 0)
            {
              *created_context = context;
              return status;
            }
        }
    }

cleanup:
  if(context->standard.creatorSignal != 0)
    {
      FreeSignal(context->standard.creatorSignal);
      context->standard.creatorSignal = 0;
    }
  if(dispose_control_subscriber(context) < 0)
    {
      *created_context = context;
    }
  return status;
}


int32
dispose_control_subscriber(RoadRashControlContext *context)
{
  int32 status;

  if(context != 0)
    {
      if(context->standard.threadItem > 0)
        {
          status = dispose_data_stream_thread(context->standard.threadItem);
          if(status < 0)
            {
              return status;
            }
          context->standard.threadItem = 0;
        }
      if(context->standard.dsReqMsgPool != 0)
        {
          destroy_memory_pool(context->standard.dsReqMsgPool);
          context->standard.dsReqMsgPool = 0;
        }
      if(context->standard.subsMsgPool != 0)
        {
          destroy_memory_pool(context->standard.subsMsgPool);
          context->standard.subsMsgPool = 0;
        }
      if(context->standard.threadStackBlock != 0)
        {
          dispose_data_stream_thread_stack(
            context->standard.threadStackBlock);
        }
      ReturnPoolMem((MemPoolPtr)ctrl_subscriber_context_pool, context);
    }
  return 0;
}


void
load_control_cue_anchors(RoadRashControlContext         *context,
                         int32                           chunk_time,
                         int32                           cue_anchor_count,
                         const RoadRashControlCueAnchor *cue_anchors)
{
  int32 anchor_index;

  (void)chunk_time;
  context->cue_anchor_count = 0;
  anchor_index = 0;
  while(anchor_index != cue_anchor_count &&
        anchor_index < CONTROL_CUE_ANCHOR_CAPACITY)
    {
      context->cue_anchors[anchor_index] = cue_anchors[anchor_index];
      context->cue_anchor_count++;
      anchor_index = context->cue_anchor_count;
    }
}


void
dispatch_due_control_cues(RoadRashControlContext    *context,
                          int32                      cue_request_count,
                          RoadRashControlCueRequest *cue_requests)
{
  RoadRashControlCueAnchor *anchor;
  RoadRashControlCueRequest *request;
  RoadRashControlCueCallback callback;
  int32 current_stream_time;
  int32 base_stream_time;
  int32 due_stream_time;
  int32 relative_stream_time;
  int32 request_index;
  int32 anchor_index;
  int32 status;

  status = DSGetClock(context->standard.streamCBPtr,
                      (uint32 *)&current_stream_time);
  if(status < 0)
    {
      return;
    }

  request = cue_requests;
  for(request_index = 0;
      request_index < cue_request_count;
      request_index++, request++)
    {
      for(anchor_index = 0;
          anchor_index < context->cue_anchor_count;
          anchor_index++)
        {
          anchor = &context->cue_anchors[anchor_index];
          if(anchor->chunk_type != request->chunk_type ||
             anchor->subchunk_type != request->subchunk_type)
            {
              continue;
            }

          if(request->use_alternate_base_time != 0)
            {
              base_stream_time =
                anchor->base_time_for_alternate_selector;
            }
          else
            {
              base_stream_time =
                anchor->base_time_for_primary_selector;
            }

          due_stream_time = (int32)(
            (uint32)request->trigger_time_offset +
            (uint32)base_stream_time);
          if(due_stream_time <= current_stream_time &&
             request->callback != 0)
            {
              callback = request->callback;
              request->callback = 0;
              relative_stream_time = (int32)(
                (uint32)current_stream_time -
                (uint32)base_stream_time);
              callback(request->callback_argument,
                       relative_stream_time);
            }
          break;
        }
    }
}

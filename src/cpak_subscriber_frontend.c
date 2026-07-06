#include "string.h"

#include "platform.h"
#include "semaphore.h"

#include "cpak_subscriber_runtime.h"
#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "subscriberutils.h"

#define CPAK_CHANNEL_INITIALIZED_FLAG 0x00010000U
#define CPAK_SUBSCRIBER_THREAD_STACK_BYTES 0x1000
#define ROAD_RASH_CPAK_RECORD_BYTES 0x88

/* LaunchMe predates the structured DataStream error-number assignments. */
#define CPAK_LEGACY_NO_MEMORY_ERROR ((int32) - 0x03E0)
#define CPAK_LEGACY_NO_SIGNAL_ERROR ((int32) - 0x03DF)
#define CPAK_LEGACY_SIGNAL_WAIT_ERROR ((int32) - 0x03DE)

typedef struct RoadRashCPakRecord
{
  CPakRec standard;
  int32 remaining_frame_count;
} RoadRashCPakRecord;

typedef char RoadRashCPakRecordSizeCheck[
    sizeof(RoadRashCPakRecord) == ROAD_RASH_CPAK_RECORD_BYTES ? 1 : -1];


SubscriberMsgPtr
dequeue_cpak_channel_message(CPakContextPtr context,
                             CPakRecPtr     record);
int32
queue_cpak_chunk_for_reply(CPakContextPtr   context,
                           SubscriberMsgPtr message);

int32
poll_cpak_chunk(CPakContextPtr    context,
                CPakRecPtr        record,
                SubsChunkDataPtr *chunk_data,
                SubscriberMsgPtr *message)
{
  *message = dequeue_cpak_channel_message(context, record);
  if(*message == 0)
    {
      return 0;
    }

  *chunk_data = (SubsChunkDataPtr)(*message)->msg.data.buffer;
  return 1;
}


int32
initialize_cpak_cel_record(DSStreamCBPtr  stream,
                           CPakContextPtr context,
                           CPakRecPtr    *created_record,
                           int32          channel_number,
                           int32          flush_on_sync)
{
  RoadRashCPakRecord *road_rash_record;
  CPakChannelPtr channel;

  road_rash_record = (RoadRashCPakRecord *)AllocPoolMem(
    (MemPoolPtr)cpak_subscriber_globals.record_pool);
  if(road_rash_record == 0)
    {
      return CPAK_LEGACY_NO_MEMORY_ERROR;
    }

  road_rash_record->remaining_frame_count = 0;
  road_rash_record->standard.curFramePtr = 0;
  road_rash_record->standard.curSubMsg = 0;
  road_rash_record->standard.channel = channel_number;
  road_rash_record->standard.streamCBPtr = stream;
  road_rash_record->standard.lastCurTime = 0;
  *created_record = &road_rash_record->standard;

  channel = &context->channel[channel_number];
  channel->fFlushOnSync = (boolean)flush_on_sync;
  channel->status &= ~CPAK_CHANNEL_INITIALIZED_FLAG;
  return 0;
}


int32
flush_cpak_cel(CPakContextPtr context,
               CPakRecPtr     record,
               int32          channel_number)
{
  SubsChunkDataPtr chunk_data;
  SubscriberMsgPtr message;

  (void)channel_number;

  if(record->curSubMsg != 0)
    {
      queue_cpak_chunk_for_reply(context, record->curSubMsg);
      record->curSubMsg = 0;
    }

  while(poll_cpak_chunk(context, record, &chunk_data, &message) != 0)
    {
      queue_cpak_chunk_for_reply(context, message);
    }

  record->curFramePtr = 0;
  return reply_freed_cpak_chunks(context);
}


int32
destroy_cpak_cel_record(CPakContextPtr context,
                        CPakRecPtr     record,
                        int32          channel_number)
{
  int32 status;

  status = flush_cpak_cel(context, record, channel_number);
  if(status < 0)
    {
      return status;
    }
  ReturnPoolMem(
    (MemPoolPtr)cpak_subscriber_globals.record_pool, record);
  return 0;
}


int32
get_cpak_current_time(CPakRecPtr record)
{
  CinePakFramePtr current_frame;

  current_frame = record->curFramePtr;
  if(current_frame == 0)
    {
      return -1;
    }
  return current_frame->time;
}


int32
signal_cpak_freed_chunks(CPakContextPtr context)
{
  int32 status;

  status = 0;
  if(context->freeListNotEmpty != 0)
    {
      status = SendSignal(context->cpakTask, context->freeQueueSignal);
      context->freeListNotEmpty = 0;
    }
  return status;
}


int32
initialize_cpak_subscriber_pools(void)
{
  cpak_subscriber_globals.context_pool = create_memory_pool(
    CPAK_MAX_SUBSCRIPTIONS, sizeof(CPakContext));
  if(cpak_subscriber_globals.context_pool == 0)
    {
      return -1;
    }

  cpak_subscriber_globals.record_pool = create_memory_pool(
    CPAK_MAX_CHANNELS, sizeof(RoadRashCPakRecord));

  if(cpak_subscriber_globals.record_pool == 0)
    {
      close_cpak_subscriber_pools();
      return CPAK_LEGACY_NO_MEMORY_ERROR;
    }
  return 0;
}


int32
close_cpak_subscriber_pools(void)
{
  destroy_memory_pool(cpak_subscriber_globals.context_pool);
  cpak_subscriber_globals.context_pool = 0;
  destroy_memory_pool(cpak_subscriber_globals.record_pool);
  cpak_subscriber_globals.record_pool = 0;
  return 0;
}


int32
create_cpak_subscriber(CPakContextPtr *created_context,
                       int32           channel_count,
                       int32           priority_delta)
{
  CPakContextPtr context;
  int32 status;
  *created_context = 0;

  context = (CPakContextPtr)AllocPoolMem(
    (MemPoolPtr)cpak_subscriber_globals.context_pool);
  if(context == 0)
    {
      return CPAK_LEGACY_NO_MEMORY_ERROR;
    }

  memset(context, 0, sizeof(CPakContext));
  context->creatorTask = CURRENTTASK->t.n_Item;
  context->numChannels = channel_count;

  context->creatorSignal = AllocSignal(0);
  if((int32)context->creatorSignal <= 0)
    {
      context->creatorSignal = 0;
      status = CPAK_LEGACY_NO_SIGNAL_ERROR;
    }
  else
    {
      status = create_data_stream_thread(
        (void *)cpak_subscriber_thread_main,
        CPAK_SUBSCRIBER_THREAD_STACK_BYTES,
        (int)(CURRENTTASK->t.n_Priority + priority_delta), 0,
        &context->threadStackBlock, 0, context);
      if(status > 0)
        {
          context->threadItem = status;
          status = wait_for_data_stream_thread_startup(
            context->threadItem, context->creatorSignal);
          if(status >= 0)
            {
              FreeSignal(context->creatorSignal);
              context->creatorSignal = 0;
              status = context->creatorStatus;
              if(status >= 0)
                {
                  *created_context = context;
                  return status;
                }
            }
        }
    }
  if(dispose_cpak_subscriber(context) < 0)
    {
      *created_context = context;
    }
  return status;
}

int32
dispose_cpak_subscriber(CPakContextPtr context)
{
  int32 status;

  if(context == 0)
    {
      return 0;
    }
  if(context->threadItem > 0)
    {
      status = dispose_data_stream_thread(context->threadItem);
      if(status < 0)
        {
          return status;
        }
      context->threadItem = 0;
    }
  if(context->creatorSignal != 0)
    {
      FreeSignal(context->creatorSignal);
      context->creatorSignal = 0;
    }
  if(context->threadStackBlock != 0)
    {
      dispose_data_stream_thread_stack(context->threadStackBlock);
      context->threadStackBlock = 0;
    }
  /* The worker's Items and task-local signals were reclaimed at exit. */
  if(context->filmCodec != 0)
    {
      DisposeCodec(context->filmCodec);
    }
  if(context->codecHndlr != 0)
    {
      DisposeCodecHandler(context->codecHndlr);
    }
  ReturnPoolMem((MemPoolPtr)cpak_subscriber_globals.context_pool, context);
  return 0;
}


SubscriberMsgPtr
dequeue_cpak_channel_message(CPakContextPtr context,
                             CPakRecPtr     record)
{
  CPakChannelPtr channel;
  SubscriberMsgPtr message;

  channel = &context->channel[record->channel];
  LockSemaphore(channel->dataQueueSem, 1);
  message = GetNextDataMsg(&channel->dataQueue);
  UnlockSemaphore(channel->dataQueueSem);
  return message;
}


int32
queue_cpak_chunk_for_reply(CPakContextPtr   context,
                           SubscriberMsgPtr message)
{
  LockSemaphore(context->freeQueueSem, 1);
  AddDataMsgToTail(&context->freeList, message);
  UnlockSemaphore(context->freeQueueSem);
  context->freeListNotEmpty = 1;
  signal_cpak_freed_chunks(context);
  return 0;
}

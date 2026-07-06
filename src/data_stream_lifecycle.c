#include "kernel.h"
#include "string.h"

#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"

enum RoadRashDataStreamLifecycleConstant
{
  ROADRASH_DATA_STREAM_THREAD_STACK_BYTES = 4096
};

static MemPoolPtr data_stream_context_pool;

int32
InitDataStreaming(long maximum_stream_count)
{
  data_stream_context_pool = create_memory_pool(
    maximum_stream_count, sizeof(DSStreamCB));
  if(data_stream_context_pool == 0)
    {
      return ROADRASH_DS_NO_MEMORY_ERROR;
    }
  return ROADRASH_DS_NO_ERROR;
}


int32
CloseDataStreaming(void)
{
  destroy_memory_pool(data_stream_context_pool);
  data_stream_context_pool = 0;
  return ROADRASH_DS_NO_ERROR;
}


int32
NewDataStream(DSStreamCBPtr *stream_result,
              void          *buffer_list,
              long           buffer_size,
              long           priority_delta,
              long           subscriber_message_count)
{
  DSStreamCBPtr stream;
  int32 status;

  stream = (DSStreamCBPtr)AllocPoolMem(data_stream_context_pool);
  if(stream == 0)
    {
      return ROADRASH_DS_NO_MEMORY_ERROR;
    }

  memset(stream, 0, sizeof(*stream));
  stream->bufDataSize = buffer_size;
  stream->freeBufHead = (DSDataBufPtr)buffer_list;
  stream->numSubsMsgs = subscriber_message_count;
  stream->threadStackBlock = 0;
  stream->creatorTask = CURRENTTASK->t.n_Item;
  stream->creatorSignal = AllocSignal(0);
  if((int32)stream->creatorSignal <= 0)
    {
      status = ROADRASH_DS_NO_SIGNAL_ERROR;
      stream->creatorSignal = 0;
      goto cleanup;
    }

  status = create_data_stream_thread(
    (void *)stream_server_thread_main,
    ROADRASH_DATA_STREAM_THREAD_STACK_BYTES,
    CURRENTTASK->t.n_Priority + priority_delta,
    0, &stream->threadStackBlock, 0, stream);
  if(status <= 0)
    {
      goto cleanup;
    }
  stream->threadItem = status;

  status = wait_for_data_stream_thread_startup(
    stream->threadItem, stream->creatorSignal);
  if(status < 0)
    {
      goto cleanup;
    }
  FreeSignal(stream->creatorSignal);
  stream->creatorSignal = 0;

  status = stream->creatorStatus;
  if(status >= 0)
    {
      *stream_result = stream;
      return status;
    }

cleanup:
  if(stream->threadItem > 0)
    {
      int32 dispose_status;

      dispose_status = dispose_data_stream_thread(stream->threadItem);
      if(dispose_status < 0)
        {
          *stream_result = stream;
          return dispose_status;
        }
      stream->threadItem = 0;
    }
  tear_down_data_stream_context(stream);
  if(stream->creatorSignal != 0)
    {
      FreeSignal(stream->creatorSignal);
      stream->creatorSignal = 0;
    }
  if(stream->threadStackBlock != 0)
    {
      dispose_data_stream_thread_stack(stream->threadStackBlock);
    }
  ReturnPoolMem(data_stream_context_pool, stream);
  return status;
}


int32
DisposeDataStream(Item          message_item,
                  DSStreamCBPtr stream)
{
  DSRequestMsg request;
  int32 status;

  status = ROADRASH_DS_NO_ERROR;
  /* A cancelled synchronous request clears threadItem only after joining
   * every consumer of the request and stream message pools. */
  if(stream->threadItem != 0)
    {
      request.whatToDo = kDSOpCloseStream;
      status = send_legacy_data_stream_request(
        message_item, false, stream, &request);
      if(status < 0)
        {
          return status;
        }
    }

  if(stream->threadItem > 0)
    {
      status = dispose_data_stream_thread(stream->threadItem);
      if(status < 0)
        {
          return status;
        }
      stream->threadItem = 0;
    }
  tear_down_data_stream_context(stream);
  if(stream->threadStackBlock != 0)
    {
      dispose_data_stream_thread_stack(stream->threadStackBlock);
    }
  ReturnPoolMem(data_stream_context_pool, stream);
  return status;
}

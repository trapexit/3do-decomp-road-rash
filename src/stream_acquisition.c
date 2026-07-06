#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "debug.h"
#include "io.h"
#include "operror.h"

#include "audio_message_runtime.h"
#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"
#include "road_rash_block_file.h"
#include "road_rash_item_pool.h"
#include "rw_semantic_data.h"
#include "stream_acquisition_runtime.h"

enum RoadRashDataAcquisitionConstant
{
  STREAM_ACQUISITION_THREAD_STACK_BYTES = 0x1000,
  STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES = 0x18,
  STREAM_ACQUISITION_READ_RETRY_COUNT = 1
};


int32
queue_stream_acquisition_request(RoadRashDataAcquisitionContextPtr context,
                                 DataAcqMsgPtr                     message)
{
  message->link = 0;
  if(context->pending_queue_head == 0)
    {
      context->pending_queue_head = message;
      context->pending_queue_tail = message;
      return 1;
    }

  context->pending_queue_tail->link = message;
  context->pending_queue_tail = message;
  return 0;
}


DataAcqMsgPtr
take_queued_stream_acquisition_request(RoadRashDataAcquisitionContextPtr context)
{
  DataAcqMsgPtr message;

  message = context->pending_queue_head;
  if(message != 0)
    {
      context->pending_queue_head = (DataAcqMsgPtr)message->link;
      if(context->pending_queue_tail == message)
        {
          context->pending_queue_tail = 0;
        }
    }
  return message;
}


int32
flush_stream_acquisition_requests(RoadRashDataAcquisitionContextPtr context,
                                  DataAcqMsgPtr                     unused_message)
{
  DataAcqMsgPtr message;
  DataAcqMsgPtr abort_tail;

  (void)unused_message;
  message = context->request_queue;
  while(message != 0)
    {
      AbortIO(message->msg.data.bufferPtr->ioreqItem);
      message = (DataAcqMsgPtr)message->link;
    }

  abort_tail = context->abort_queue;
  if(abort_tail != 0)
    {
      while(abort_tail->link != 0)
        {
          abort_tail = (DataAcqMsgPtr)abort_tail->link;
        }
      abort_tail->link = context->request_queue;
    }
  else
    {
      context->abort_queue = context->request_queue;
    }
  context->request_queue = 0;

  while((message = take_queued_stream_acquisition_request(context)) != 0)
    {
      ReplyMsg(message->msgItem, ROADRASH_DS_WAS_FLUSHED_ERROR, message,
               STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES);
    }
  return ROADRASH_DS_NO_ERROR;
}


int32
initialize_stream_acquisition_context_pool(int32 context_count)
{
  void *pool;

  pool = create_memory_pool(
    context_count, ROADRASH_STREAM_ACQUISITION_CONTEXT_BYTES);
  stream_acquisition_context_pool = pool;
  if(pool != 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }
  return ROADRASH_DS_NO_MEMORY_ERROR;
}


int32
dispose_stream_acquisition_context_pool(void)
{
  destroy_memory_pool(stream_acquisition_context_pool);
  stream_acquisition_context_pool = 0;
  return ROADRASH_DS_NO_ERROR;
}


static
void
release_stream_acquisition_resources(RoadRashDataAcquisitionContextPtr context)
{
  if(context->io_request_pool != 0)
    {
      RoadRashDeleteItemPool(context->io_request_pool);
      context->io_request_pool = 0;
    }
  close_stream_acquisition_file(context);
  if(context->request_port > 0)
    {
      DeleteItem(context->request_port);
      context->request_port = 0;
    }
  if(context->io_done_reply_port > 0)
    {
      DeleteItem(context->io_done_reply_port);
      context->io_done_reply_port = 0;
    }
}


int32
create_stream_acquisition_context(RoadRashDataAcquisitionContextPtr *created_context,
                                  char                              *file_name,
                                  int32                              priority_delta)
{
  RoadRashDataAcquisitionContextPtr context;
  int32 status;
  int32 signal;

  *created_context = 0;
  context = (RoadRashDataAcquisitionContextPtr)
            AllocPoolMem((MemPoolPtr)stream_acquisition_context_pool);
  if(context == 0)
    {
      return ROADRASH_DS_NO_MEMORY_ERROR;
    }

  memset(context, 0, sizeof(*context));
  context->file_name = file_name;
  context->creator_task = CURRENTTASK->t.n_Item;
  context->creator_status = ROADRASH_DS_INITIALIZATION_ERROR;
  signal = AllocSignal(0);
  if(signal <= 0)
    {
      status = ROADRASH_DS_NO_SIGNAL_ERROR;
    }
  else
    {
      context->creator_signal = signal;
      status = create_data_stream_thread(
        (void *)stream_acquisition_thread_main,
        STREAM_ACQUISITION_THREAD_STACK_BYTES,
        CURRENTTASK->t.n_Priority + priority_delta, 0,
        &context->thread_stack, 0, context);
      if(status > 0)
        {
          context->thread_item = status;
          status = wait_for_data_stream_thread_startup(
            context->thread_item, context->creator_signal);
          if(status >= 0)
            {
              FreeSignal(signal);
              context->creator_signal = 0;
              status = context->creator_status;
              if(status >= 0)
                {
                  *created_context = context;
                  return status;
                }
            }
        }
    }

  if(dispose_stream_acquisition_context(context) < 0)
    {
      *created_context = context;
    }
  return status;
}


int32
dispose_stream_acquisition_context(RoadRashDataAcquisitionContextPtr context)
{
  int32 status;

  if(context == 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }
  /* The stream must return every buffer before its acquisition is destroyed. */
  if(context->request_queue != 0 || context->abort_queue != 0 ||
     context->pending_queue_head != 0)
    {
      return ROADRASH_DS_INVALID_REQUEST_ERROR;
    }

  status = dispose_data_stream_thread(context->thread_item);
  if(status < 0)
    {
      return status;
    }
  context->thread_item = 0;
  if(context->creator_signal != 0)
    {
      FreeSignal(context->creator_signal);
      context->creator_signal = 0;
    }
  if(context->thread_stack != 0)
    {
      dispose_data_stream_thread_stack(context->thread_stack);
      context->thread_stack = 0;
    }
  release_stream_acquisition_resources(context);
  ReturnPoolMem((MemPoolPtr)stream_acquisition_context_pool, context);
  return ROADRASH_DS_NO_ERROR;
}


int32
connect_stream_acquisition(RoadRashDataAcquisitionContextPtr context,
                           DataAcqMsgPtr                     message)
{
  (void)context;
  (void)message;
  return ROADRASH_DS_NO_ERROR;
}


int32
disconnect_stream_acquisition(RoadRashDataAcquisitionContextPtr context,
                              DataAcqMsgPtr                     message)
{
  flush_stream_acquisition_requests(context, message);
  return ROADRASH_DS_NO_ERROR;
}


int32
map_stream_acquisition_marker(RoadRashDataAcquisitionContextPtr context,
                              uint32                            marker_value,
                              int32                             marker_options,
                              int32                            *file_offset)
{
  int32 file_size;

  (void)marker_options;
  file_size = GetBlockFileSize(&context->block_file);
  if((uint32)file_size > marker_value)
    {
      *file_offset = marker_value;
      return ROADRASH_DS_NO_ERROR;
    }
  return ROADRASH_DS_END_OF_FILE_ERROR;
}


int32
seek_stream_acquisition(RoadRashDataAcquisitionContextPtr context,
                        DataAcqMsgPtr                     message)
{
  int32 file_offset;
  int32 status;

  status = map_stream_acquisition_marker(
    context, message->msg.marker.value,
    message->msg.marker.options, &file_offset);
  if(status != ROADRASH_DS_NO_ERROR)
    {
      return status;
    }

  flush_stream_acquisition_requests(context, message);
  context->file_offset = file_offset;
  context->expected_completed_offset = file_offset;
  context->completed_offset_error_seen = 0;
  context->end_of_file_sent = false;
  return ROADRASH_DS_NO_ERROR;
}


int32
request_stream_acquisition_read(RoadRashDataAcquisitionContextPtr context,
                                DataAcqMsgPtr                     message)
{
  DSDataBufPtr buffer;
  DataAcqMsgPtr queue_tail;
  int32 status;

  buffer = message->msg.data.bufferPtr;
  if(GetBlockFileSize(&context->block_file) <= context->file_offset)
    {
      status = context->end_of_file_sent
                     ? ROADRASH_DS_WAS_FLUSHED_ERROR
                     : ROADRASH_DS_END_OF_FILE_ERROR;
      context->end_of_file_sent = true;
      return status;
    }

  buffer->ioreqItem = RoadRashAllocPoolItem(context->io_request_pool);
  if(buffer->ioreqItem == 0)
    {
      queue_stream_acquisition_request(context, message);
      return ROADRASH_DS_NO_ERROR;
    }

  buffer->ioreqItemPtr = (IOReq *)LookupItem(buffer->ioreqItem);
  status = AsynchReadBlockFile(
    &context->block_file, buffer->ioreqItem,
    (void *)&buffer->streamData, message->msg.data.bufferSize,
    context->file_offset);
  if(status < 0)
    {
      RoadRashReturnPoolItem(context->io_request_pool, buffer->ioreqItem);
      buffer->ioreqItem = 0;
      buffer->ioreqItemPtr = 0;
      return status;
    }

  context->file_offset += message->msg.data.bufferSize;
  /* Keep submission order; CD completions may arrive out of order. */
  queue_tail = context->request_queue;
  if(queue_tail != 0)
    {
      while(queue_tail->link != 0)
        {
          queue_tail = (DataAcqMsgPtr)queue_tail->link;
        }
      queue_tail->link = message;
    }
  else
    {
      context->request_queue = message;
    }
  message->link = 0;
  return status;
}


int32
initialize_stream_acquisition_thread(RoadRashDataAcquisitionContextPtr context)
{
  int32 status;

  context->creator_status = -1;
  context->abort_queue = 0;
  context->request_port = 0;
  context->io_done_reply_port = 0;
  context->request_queue = 0;
  context->end_of_file_sent = false;
  context->file_offset = 0;
  context->expected_completed_offset = 0;

  context->completed_offset_error_seen = 0;
  context->io_request_pool = 0;
  context->pending_queue_head = 0;
  context->pending_queue_tail = 0;

  status = NewMsgPort(&context->request_port_signal);
  if(status > 0)
    {
      context->request_port = status;
      status = NewMsgPort(&context->io_done_reply_port_signal);
      if(status > 0)
        {
          context->io_done_reply_port = status;
          status = open_stream_acquisition_file(context);
          context->creator_status = status;
        }
    }
  if(status < 0)
    {
      context->creator_status = status;
    }

  status = SendSignal(context->creator_task, context->creator_signal);
  if(context->creator_status >= 0 && status >= 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }
  return -1;
}


Item
create_stream_acquisition_io_request(void *create_argument)
{
  RoadRashDataAcquisitionContextPtr context;

  context = (RoadRashDataAcquisitionContextPtr)create_argument;
  return CreateBlockFileIOReq(
    context->block_file.fDevice, context->io_done_reply_port);
}


int32
open_stream_acquisition_file(RoadRashDataAcquisitionContextPtr context)
{
  int32 status;

  context->block_file.fDevice = 0;
  status = RoadRashOpenBlockFile(
    context->file_name, &context->block_file);
  if(status == ROADRASH_DS_NO_ERROR)
    {
      context->io_request_pool = RoadRashCreateItemPool(
        ROADRASH_STREAM_ACQUISITION_IO_REQUEST_COUNT,
        create_stream_acquisition_io_request, context);
      if(context->io_request_pool == 0)
        {
          close_stream_acquisition_file(context);
          return ROADRASH_DS_NO_MEMORY_ERROR;
        }
    }
  return status;
}


void
close_stream_acquisition_file(RoadRashDataAcquisitionContextPtr context)
{
  if(context->block_file.fDevice != 0)
    {
      CloseBlockFile(&context->block_file);
      context->block_file.fDevice = 0;
    }
}


void
stream_acquisition_thread_main(int32                             ignored_initial_argument,
                               RoadRashDataAcquisitionContextPtr context)
{
  DataAcqMsgPtr message;
  DataAcqMsgPtr pending_message;
  DataAcqMsgPtr exit_request;
  DSDataBufPtr buffer;
  IOReq *io_request;
  Message *completion;
  int32 status;
  uint32 awaited_signals;
  uint32 received_signals;
  Item io_request_item;
  int32 retry_count;
  int32 queue_result;
  int32 actual_offset;
  boolean exit_requested;
  boolean read_aborted;
  int32 opcode;

  (void)ignored_initial_argument;
  message = 0;
  buffer = 0;
  io_request = 0;
  pending_message = 0;
  exit_request = 0;
  status = initialize_stream_acquisition_thread(context);
  if(status != ROADRASH_DS_NO_ERROR)
    {
      release_stream_acquisition_resources(context);
      exit(0);
    }

  awaited_signals = context->request_port_signal |
                    context->io_done_reply_port_signal;
  exit_requested = false;

  for(;;)
    {
      received_signals = WaitSignal(awaited_signals);

      if((received_signals & context->io_done_reply_port_signal) != 0)
        {
          for(;;)
            {
              read_aborted = context->abort_queue != 0;
              message = read_aborted ? context->abort_queue
                                     : context->request_queue;
              if(message == 0)
                {
                  break;
                }
              buffer = message->msg.data.bufferPtr;
              io_request = buffer->ioreqItemPtr;
              completion = (Message *)LookupItem(io_request->io_MsgItem);
              /* Leave later replies queued until the oldest read completes. */
              if(completion == 0 ||
                 completion->msg_MsgPort != context->io_done_reply_port ||
                 (completion->msg.n_Flags & MESSAGE_REPLIED) == 0 ||
                 GetThisMsg(io_request->io_MsgItem) <= 0)
                {
                  break;
                }
              io_request_item = buffer->ioreqItem;
              if(read_aborted)
                {
                  context->abort_queue = (DataAcqMsgPtr)message->link;
                  status = ROADRASH_DS_WAS_FLUSHED_ERROR;
                }
              else
                {
                  context->request_queue = (DataAcqMsgPtr)message->link;
                }

              if(!read_aborted)
                {
                  retry_count = 0;

check_read:
                  buffer = message->msg.data.bufferPtr;
                  io_request = buffer->ioreqItemPtr;
                  status = io_request->io_Error;

                  if(status != ROADRASH_DS_NO_ERROR)
                    {
                      kprintf("DataAcq read error %ld\n", status);
                      PrintfSysErr(status);

                      if(retry_count <
                         STREAM_ACQUISITION_READ_RETRY_COUNT)
                        {
                          retry_count++;
                          queue_result = AsynchReadBlockFile(
                            &context->block_file, io_request_item,
                            io_request->io_Info.ioi_Recv.iob_Buffer,
                            io_request->io_Info.ioi_Recv.iob_Len,
                            context->expected_completed_offset);
                          if(queue_result >= ROADRASH_DS_NO_ERROR)
                            {
                              WaitIO(io_request_item);
                            }
                          goto check_read;
                        }
                    }
                  else
                    {
                      actual_offset =
                        io_request->io_Info.ioi_Offset *
                        context->block_file.fStatus.fs.ds_DeviceBlockSize;
                      if(actual_offset ==
                         context->expected_completed_offset)
                        {
                          context->expected_completed_offset +=
                            io_request->io_Actual;
                        }
                      else
                        {
                          if(context->completed_offset_error_seen == 0)
                            {
                              status = ROADRASH_DS_ABORT_ERROR;
                              context->completed_offset_error_seen = 1;
                            }
                          kprintf(
                            "DataAcq found block offset %ld, but expected %ld\n",
                            actual_offset,
                            context->expected_completed_offset);
                        }
                    }
                }

              ReplyMsg(message->msgItem, status, message,
                       STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES);
              RoadRashReturnPoolItem(
                context->io_request_pool, io_request_item);

              pending_message =
                take_queued_stream_acquisition_request(context);
              if(pending_message != 0)
                {
                  status = request_stream_acquisition_read(
                    context, pending_message);
                  if(status < ROADRASH_DS_NO_ERROR)
                    {
                      ReplyMsg(
                        pending_message->msgItem, status,
                        pending_message,
                        STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES);
                    }
                }
            }
        }

      if((received_signals & context->request_port_signal) != 0)
        {
          while(PollForMsg(context->request_port, 0, 0,
                           (void **)&message, &status) != 0)
            {
              opcode = message->whatToDo;
              switch(opcode)
                {
                case kAcqOpGetData:
                  status = request_stream_acquisition_read(context, message);
                  if(status < ROADRASH_DS_NO_ERROR)
                    {
                      ReplyMsg(
                        message->msgItem, status, message,
                        STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES);
                    }
                  break;
                case kAcqOpGoMarker:
                  status = seek_stream_acquisition(context, message);
                  break;
                case kAcqOpConnect:
                  status = connect_stream_acquisition(context, message);
                  break;
                case kAcqOpDisconnect:
                  status = disconnect_stream_acquisition(
                    context, message);
                  break;
                case kAcqOpExit:
                  exit_requested = true;
                  if(exit_request == 0)
                    {
                      exit_request = message;
                    }
                  else
                    {
                      kprintf(
                        "DataAcqThread got multiple kAcqOpExit messages!\n");
                    }
                  break;
                }

              if(opcode != kAcqOpGetData && opcode != kAcqOpExit)
                {
                  ReplyMsg(
                    message->msgItem, status, message,
                    STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES);
                }
            }
        }

      if(exit_requested != false && context->abort_queue == 0 &&
         context->request_queue == 0)
        {
          ReplyMsg(exit_request->msgItem, ROADRASH_DS_NO_ERROR,
                   exit_request,
                   STREAM_ACQUISITION_COMPLETION_MESSAGE_BYTES);
          break;
        }
    }

  release_stream_acquisition_resources(context);
  exit(0);
}

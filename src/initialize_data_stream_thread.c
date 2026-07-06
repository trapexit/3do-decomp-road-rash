/* Portfolio 1.2 DataStreamer server reconstructed from the embedded copy. */

#include "platform.h"
#include "stdlib.h"

#include "audio.h"
#include "sdk_audio_compat.h"
#include "audio_message_runtime.h"
#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"

#define STREAM_BUFFER_ALIGNMENT_MASK (sizeof(uint32) - 1)

enum
{
  STREAM_ADDITIONAL_ACQUISITION_MESSAGES = 6
};

int32
initialize_data_stream_thread(DSStreamCBPtr stream)
{
  DSDataBufPtr buffer;
  long buffer_count;
  int32 status;

  stream->creatorStatus = ROADRASH_DS_INITIALIZATION_ERROR;
  stream->streamFlags = 0;
  stream->clockOffset = 0;
  stream->filledBufHead = 0;
  stream->filledBufTail = 0;
  stream->acquirePort = 0;
  stream->repliesPending = 0;
  stream->requestMsgHead = 0;
  stream->requestMsgTail = 0;
  stream->numSubscribers = 0;
  stream->subsMsgPool = 0;
  stream->dataMsgPool = 0;

  buffer_count = 0;
  buffer = stream->freeBufHead;
  while(buffer != 0)
    {
      if(((uint32)buffer & STREAM_BUFFER_ALIGNMENT_MASK) != 0)
        {
          stream->creatorStatus = ROADRASH_DS_BAD_BUFFER_ALIGNMENT_ERROR;
          goto signal_creator;
        }
      buffer->streamCBPtr = stream;
      buffer_count++;
      buffer = buffer->next;
    }

  stream->totalBufferCount = buffer_count;
  stream->currentFreeBufferCount = buffer_count;
  stream->endOfStreamMsg = 0;

  status = OpenAudioFolio();
  if(status < 0)
    {
      stream->creatorStatus = status;
      goto signal_creator;
    }

  stream->creatorStatus = ROADRASH_DS_NO_PORT_ERROR;
  stream->requestPort = NewMsgPort(&stream->requestPortSignal);
  if(stream->requestPort <= 0)
    {
      goto signal_creator;
    }

  stream->acqReplyPort = NewMsgPort(&stream->acqReplyPortSignal);
  if(stream->acqReplyPort <= 0)
    {
      goto signal_creator;
    }

  stream->subsReplyPort = NewMsgPort(&stream->subsReplyPortSignal);
  if(stream->subsReplyPort <= 0)
    {
      goto signal_creator;
    }

  stream->creatorStatus = ROADRASH_DS_NO_MEMORY_ERROR;
  stream->subsMsgPool = (MemPoolPtr)create_memory_pool(
    stream->numSubsMsgs, sizeof(SubscriberMsg));
  if(stream->subsMsgPool == 0)
    {
      goto signal_creator;
    }

  stream->dataMsgPool = (MemPoolPtr)create_memory_pool(
    buffer_count + STREAM_ADDITIONAL_ACQUISITION_MESSAGES,
    sizeof(DataAcqMsg));
  if(stream->dataMsgPool == 0)
    {
      goto signal_creator;
    }

  stream->creatorStatus = ROADRASH_DS_NO_MESSAGE_ERROR;
  if(FillPoolWithMsgItems(stream->subsMsgPool,
                          stream->subsReplyPort) == false)
    {
      goto signal_creator;
    }
  if(FillPoolWithMsgItems(stream->dataMsgPool,
                          stream->acqReplyPort) == false)
    {
      goto signal_creator;
    }
  stream->creatorStatus = ROADRASH_DS_NO_ERROR;

signal_creator:
  status = SendSignal(stream->creatorTask, stream->creatorSignal);
  if(status >= 0)
    {
      status = stream->creatorStatus;
    }
  if(status < 0)
    {
      return status;
    }
  return ROADRASH_DS_NO_ERROR;
}


void
stream_server_thread_main(int32         ignored_initial_argument,
                          DSStreamCBPtr stream)
{
  DSRequestMsgPtr request;
  DataAcqMsgPtr acquisition_reply;
  SubscriberMsgPtr subscriber_reply;
  DSDataBufPtr buffer;
  Item message_item;
  Message *message;
  uint32 awaited_signals;
  uint32 received_signals;
  int32 status;
  /* Keep the head request's result here, never in its kernel-owned Message. */
  int32 request_result;
  boolean keep_running;

  (void)ignored_initial_argument;
  if(initialize_data_stream_thread(stream) != ROADRASH_DS_NO_ERROR)
    {
      exit(0);
    }

  awaited_signals = stream->requestPortSignal |
                    stream->acqReplyPortSignal |
                    stream->subsReplyPortSignal;
  keep_running = true;
  request_result = ROADRASH_DS_NO_ERROR;

  for(;;)
    {
      received_signals = WaitSignal(awaited_signals);

      if((received_signals & stream->requestPortSignal) != 0)
        {
          while(PollForMsg(stream->requestPort, 0, 0,
                           (void **)&request, &status) != 0)
            {
              dispatch_data_stream_request(
                stream, request, &keep_running, &request_result);
            }
        }

      if((received_signals & stream->subsReplyPortSignal) != 0)
        {
          while(PollForMsg(stream->subsReplyPort, &message_item, &message,
                           (void **)&subscriber_reply, &status) != 0)
            {
              handle_data_stream_subscriber_reply(
                stream, subscriber_reply, message, &request_result);
            }
        }

      if((received_signals & stream->acqReplyPortSignal) != 0)
        {
          while(PollForMsg(stream->acqReplyPort, &message_item, &message,
                           (void **)&acquisition_reply, &status) != 0)
            {
              handle_data_stream_acquisition_reply(
                stream, acquisition_reply, message, &request_result);
            }
        }

      status = deliver_data_stream_buffers(stream);

      if(stream->acquirePort != 0 &&
         (stream->streamFlags & STRM_RUNNING) != 0 &&
         (stream->streamFlags & STRM_EOF) == 0)
        {
          buffer = take_free_data_stream_buffer(stream);
          while(buffer != 0)
            {
              status = request_data_stream_buffer_fill(stream, buffer);
              if(status < 0)
                {
                  abort_data_stream(stream);
                  break;
                }
              buffer = take_free_data_stream_buffer(stream);
            }
        }
closing_check:
      if(!keep_running)
        {
          buffer = take_filled_data_stream_buffer(stream);
          while(buffer != 0)
            {
              return_data_stream_buffer(stream, buffer);
              buffer = take_filled_data_stream_buffer(stream);
            }
          if(stream->repliesPending == 0 &&
             stream->dataMsgPool->numFreeInPool ==
               stream->dataMsgPool->numItemsInPool &&
             stream->subsMsgPool->numFreeInPool ==
               stream->subsMsgPool->numItemsInPool)
            {
              request = take_next_data_stream_request(stream);
              if(stream->endOfStreamMsg != 0)
                {
                  ReplyMsg(stream->endOfStreamMsg->msgItem,
                           ROADRASH_DS_ABORT_ERROR,
                           stream->endOfStreamMsg, sizeof(DSRequestMsg));
                  stream->endOfStreamMsg = 0;
                }
              ReplyMsg(request->msgItem, request_result, request,
                       sizeof(DSRequestMsg));
              goto thread_exit;
            }
          continue;
        }

      while(stream->repliesPending == 0 && stream->requestMsgHead != 0)
        {
          request = stream->requestMsgHead;
          status = dispatch_data_stream_request(
            stream, request, &keep_running, &request_result);
          if(!keep_running)
            {
              goto closing_check;
            }
        }
    }

thread_exit:
  exit(0);
}

#include "debug.h"
#include "mempool.h"
#include "platform.h"

#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

#define STREAM_BUFFER_TRAILING_GUARD_BYTES \
        (sizeof(StreamChunk) + \
         sizeof(((StreamChunkPtr)0)->streamChunkData))

int32
deliver_data_stream_buffers(DSStreamCBPtr stream)
{
  DSDataBufPtr buffer;
  StreamChunkPtr chunk;
  SubscriberMsgPtr message;
  DSSubscriberPtr subscriber;
  char *buffer_data_end;
  char *buffer_end;
  int32 remaining_bytes;
  int32 result;
  int32 chunk_size;

  if((stream->streamFlags & STRM_RUNNING) == 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }

  result = ROADRASH_DS_NO_ERROR;
  buffer = take_filled_data_stream_buffer(stream);
  while(buffer != 0)
    {
      buffer->useCount = 0;
      chunk = (StreamChunkPtr)buffer->streamData;
      if(stream->bufDataSize <= STREAM_BUFFER_TRAILING_GUARD_BYTES)
        {
          abort_data_stream(stream);
          return_data_stream_buffer(stream, buffer);
          return ROADRASH_DS_INVALID_CHUNK_ERROR;
        }
      buffer_data_end =
        buffer->streamData + stream->bufDataSize;
      buffer_end =
        buffer_data_end - STREAM_BUFFER_TRAILING_GUARD_BYTES;

      while(buffer_end > (char *)chunk)
        {
          chunk_size = chunk->streamChunkSize;
          remaining_bytes =
            (int32)(buffer_data_end - (char *)chunk);
          if((chunk_size & (sizeof(uint32) - 1)) != 0 ||
             chunk_size <= 0 ||
             chunk_size > remaining_bytes)
            {
              kprintf(
                "Invalid chunk in data (chunk size = 0x%lx)\n",
                chunk_size);
              abort_data_stream(stream);
              if(buffer->useCount == 0)
                {
                  return_data_stream_buffer(stream, buffer);
                }
              return ROADRASH_DS_INVALID_CHUNK_ERROR;
            }

          if(find_data_stream_subscriber(
               stream, chunk->streamChunkType,
               &subscriber) == ROADRASH_DS_NO_ERROR)
            {
              message = (SubscriberMsgPtr)AllocPoolMem(
                stream->subsMsgPool);
              if(message != 0)
                {
                  message->privatePtr = buffer;
                  message->msg.data.buffer = chunk;
                  message->whatToDo = kStreamOpData;
                  result = SendMsg(
                    subscriber->subscriberPort,
                    message->msgItem, message,
                    sizeof(SubscriberMsg));
                  if(result < 0)
                    {
                      ReturnPoolMem(stream->subsMsgPool, message);
                    }
                }
              else
                {
                  result = ROADRASH_DS_NO_MESSAGE_ERROR;
                }

              if(result < 0)
                {
                  abort_data_stream(stream);
                  if(buffer->useCount == 0)
                    {
                      return_data_stream_buffer(stream, buffer);
                    }
                  return result;
                }
              buffer->useCount++;
            }
          else
            {
              result = ROADRASH_DS_NO_ERROR;
            }

          chunk = (StreamChunkPtr)((char *)chunk + chunk_size);
        }

      if(buffer->useCount == 0)
        {
          result = request_data_stream_buffer_fill(stream, buffer);
        }

      buffer = take_filled_data_stream_buffer(stream);
    }

  return result;
}


#undef STREAM_BUFFER_TRAILING_GUARD_BYTES

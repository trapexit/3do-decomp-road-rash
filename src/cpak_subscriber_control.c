#include "stdlib.h"
#include "string.h"

#include "audio.h"
#include "codec.h"
#include "item.h"
#include "sdk_audio_compat.h"
#include "semaphore.h"
#include "task.h"

#include "audio_message_runtime.h"
#include "cpak_subscriber_runtime.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "subscriberutils.h"

static
void
stop_cpak_channel(CPakContextPtr context,
                  long           channel_number)
{
  CPakChannelPtr channel;

  if(channel_number < CPAK_MAX_CHANNELS)
    {
      channel = &context->channel[channel_number];
      channel->status &= ~CHAN_ENABLED;
    }
}


static
int32
flush_cpak_channel(CPakContextPtr context,
                   long           channel_number)
{
  SubscriberMsgPtr message;
  SubscriberMsgPtr next_message;
  CPakChannelPtr channel;

  if(channel_number < CPAK_MAX_CHANNELS)
    {
      channel = &context->channel[channel_number];
      LockSemaphore(channel->dataQueueSem, 1);

      message = channel->dataQueue.head;
      while(message != 0)
        {
          next_message = (SubscriberMsgPtr)message->link;
          ReplyMsg(message->msgItem, 0, message,
                   sizeof(SubscriberMsgPtr));
          message = next_message;
        }

      channel->dataQueue.head = 0;
      channel->dataQueue.tail = 0;
      return UnlockSemaphore(channel->dataQueueSem);
    }
  return 0;
}


static
int32
queue_cpak_data_message(CPakContextPtr   context,
                        SubscriberMsgPtr message)
{
  SubsChunkDataPtr chunk_data;
  CPakChannelPtr channel;
  long channel_number;

  chunk_data = (SubsChunkDataPtr)message->msg.data.buffer;
  channel_number = chunk_data->channel;

  if(channel_number < context->numChannels)
    {
      channel = &context->channel[channel_number];
      AddDataMsgToTail(&channel->dataQueue, message);
    }
  else
    {
      ReplyMsg(message->msgItem, 0, message, 0);
    }

  return 0;
}


static
int32
get_cpak_channel_status(CPakContextPtr   context,
                        SubscriberMsgPtr message)
{
  long channel_number;

  channel_number = message->msg.channel.number;
  if(channel_number < CPAK_MAX_CHANNELS)
    {
      return context->channel[channel_number].status;
    }
  return 0;
}


static
int32
set_cpak_channel_subscription(CPakContextPtr   context,
                              SubscriberMsgPtr message)
{
  long channel_number;

  channel_number = message->msg.channel.number;
  if(channel_number < CPAK_MAX_CHANNELS)
    {
      context->channel[channel_number].status |=
        ~CHAN_SYSBITS & message->msg.channel.status;
    }
  return 0;
}


static
int32
handle_cpak_control_message(CPakContextPtr   context,
                            SubscriberMsgPtr message)
{
  (void)context;
  (void)message;
  return 0;
}


static
int32
acknowledge_cpak_stream_opening(CPakContextPtr   context,
                                SubscriberMsgPtr message)
{
  (void)context;
  (void)message;
  return 0;
}


static
int32
acknowledge_cpak_stream_closing(CPakContextPtr   context,
                                SubscriberMsgPtr message)
{
  (void)context;
  (void)message;
  return 0;
}


static
int32
stop_cpak_subscription(CPakContextPtr   context,
                       SubscriberMsgPtr message)
{
  long channel_number;

  (void)message;
  for(channel_number = 0;
      channel_number < CPAK_MAX_CHANNELS;
      channel_number++)
    {
      stop_cpak_channel(context, channel_number);
    }
  return 0;
}


static
int32
synchronize_cpak_subscription(CPakContextPtr   context,
                              SubscriberMsgPtr message)
{
  CPakChannelPtr channel;
  long channel_number;

  (void)message;
  reply_freed_cpak_chunks(context);
  for(channel_number = 0;
      channel_number < CPAK_MAX_CHANNELS;
      channel_number++)
    {
      channel = &context->channel[channel_number];
      if(channel->fFlushOnSync == 1)
        {
          flush_cpak_channel(context, channel_number);
        }
    }
  context->fTimeChanged = 1;
  return 0;
}


static
int32
acknowledge_cpak_end_of_file(CPakContextPtr   context,
                             SubscriberMsgPtr message)
{
  (void)context;
  (void)message;
  return 0;
}


static
int32
abort_cpak_subscription(CPakContextPtr   context,
                        SubscriberMsgPtr message)
{
  long channel_number;

  (void)message;
  for(channel_number = 0;
      channel_number < CPAK_MAX_CHANNELS;
      channel_number++)
    {
      flush_cpak_channel(context, channel_number);
      stop_cpak_channel(context, channel_number);
    }
  return 0;
}


int32
reply_freed_cpak_chunks(CPakContextPtr context)
{
  SubscriberMsgPtr message;

  LockSemaphore(context->freeQueueSem, 1);
  message = GetNextDataMsg(&context->freeList);
  while(message != 0)
    {
      ReplyMsg(message->msgItem, 0, message, 0);
      message = GetNextDataMsg(&context->freeList);
    }
  return UnlockSemaphore(context->freeQueueSem);
}

enum CPakSubscriberThreadInitFailure
{
  CPAK_THREAD_INIT_CODEC_HANDLER_FAILED = -11,
  CPAK_THREAD_INIT_CODEC_FAILED = -12,
  CPAK_THREAD_INIT_REQUEST_PORT_FAILED = -13,
  CPAK_THREAD_INIT_CUE_FAILED = -14,
  CPAK_THREAD_INIT_SIGNAL_FAILED = -15
};

int32
initialize_cpak_subscriber_thread(CPakContextPtr context)
{
  char semaphore_name[CPAK_SEMAPHORE_NAME_BYTES];
  char channel_number_text[CPAK_CHANNEL_NUMBER_BYTES];
  CPakChannelPtr channel;
  int32 status;
  long channel_number;
  int audio_opened;

  context->creatorStatus = -1;
  context->localTimeOrigin = 0;
  context->requestPort = 0;
  context->cueItem = 0;
  context->cueSignal = 0;
  audio_opened = 0;

  status = OpenAudioFolio();
  if(status < 0)
    {
      context->creatorStatus = status;
      goto cleanup_init;
    }
  audio_opened = 1;

  context->cpakTask = CURRENTTASK->t.n_Item;
  context->freeList.head = 0;
  context->freeQueueSem =
    CreateSemaphore("freeCPakQueue", CPAK_QUEUE_SEMAPHORE_PRIORITY);
  if(context->freeQueueSem < 0)
    {
      context->creatorStatus = context->freeQueueSem;
      context->freeQueueSem = 0;
      goto cleanup_init;
    }
  context->freeQueueSignal = AllocSignal(0);
  if((int32)context->freeQueueSignal <= 0)
    {
      context->creatorStatus = CPAK_THREAD_INIT_SIGNAL_FAILED;
      context->freeQueueSignal = 0;
      goto cleanup_init;
    }
  context->freeListNotEmpty = 0;

  context->codecHndlr = CreateCodecHandler();
  if(context->codecHndlr == 0)
    {
      context->creatorStatus = CPAK_THREAD_INIT_CODEC_HANDLER_FAILED;
      goto cleanup_init;
    }

  context->filmCodec = CreateCodec(context->codecHndlr, 0);
  if(context->filmCodec == 0)
    {
      context->creatorStatus = CPAK_THREAD_INIT_CODEC_FAILED;
      goto cleanup_init;
    }

  channel = context->channel;
  for(channel_number = 0;
      channel_number < CPAK_MAX_CHANNELS;
      channel_number++, channel++)
    {
      channel->status = CHAN_ENABLED;
      channel->dataQueue.head = 0;
      channel->dataQueue.tail = 0;
      channel->fFlushOnSync = 0;
      channel->imageData.baseAddr = 0;

      semaphore_name[0] = 'c';
      semaphore_name[1] = 'p';
      semaphore_name[CPAK_SEMAPHORE_PREFIX_TERMINATOR_INDEX] = 0;
      channel_number_text[0] = (char)('0' + channel_number);
      channel_number_text[1] = 0;
      strcat(semaphore_name, channel_number_text);
      channel->dataQueueSem =
        CreateSemaphore(semaphore_name,
                        CPAK_QUEUE_SEMAPHORE_PRIORITY);
      if(channel->dataQueueSem < 0)
        {
          context->creatorStatus = channel->dataQueueSem;
          channel->dataQueueSem = 0;
          goto cleanup_init;
        }
    }

  status = NewMsgPort(&context->requestPortSignal);
  if(status <= 0)
    {
      context->creatorStatus = CPAK_THREAD_INIT_REQUEST_PORT_FAILED;
      goto cleanup_init;
    }
  context->requestPort = status;

  status = CreateSizedItem(MKNODEID(AUDIONODE, AUDIO_CUE_NODE), 0, 0);
  if(status <= 0)
    {
      context->creatorStatus = CPAK_THREAD_INIT_CUE_FAILED;
      goto cleanup_init;
    }
  context->cueItem = status;

  context->cueSignal = GetCueSignal(context->cueItem);
  if(context->cueSignal != 0)
    {
      context->creatorStatus = 0;
      goto signal_creator;
    }
cleanup_init:
  channel = context->channel;
  for(channel_number = 0;
      channel_number < CPAK_MAX_CHANNELS;
      channel_number++, channel++)
    {
      if(channel->dataQueueSem > 0)
        {
          DeleteItem(channel->dataQueueSem);
          channel->dataQueueSem = 0;
        }
    }
  if(context->cueItem > 0)
    {
      DeleteItem(context->cueItem);
      context->cueItem = 0;
    }
  if(context->requestPort > 0)
    {
      DeleteItem(context->requestPort);
      context->requestPort = 0;
    }
  if(context->filmCodec != 0)
    {
      DisposeCodec(context->filmCodec);
      context->filmCodec = 0;
    }
  if(context->codecHndlr != 0)
    {
      DisposeCodecHandler(context->codecHndlr);
      context->codecHndlr = 0;
    }
  if(context->freeQueueSem > 0)
    {
      DeleteItem(context->freeQueueSem);
      context->freeQueueSem = 0;
    }
  if(context->freeQueueSignal != 0)
    {
      FreeSignal(context->freeQueueSignal);
      context->freeQueueSignal = 0;
    }
  if(audio_opened != 0)
    {
      CloseAudioFolio();
    }

signal_creator:
  status = SendSignal(context->creatorTask, context->creatorSignal);
  if(context->creatorStatus < 0 || status < 0)
    {
      return -1;
    }
  return 0;
}


void
cpak_subscriber_thread_main(int32          ignored_initial_argument,
                            CPakContextPtr context)
{
  SubscriberMsgPtr message;
  uint32 awaited_signals;
  uint32 received_signals;
  int32 status;
  int keep_running;
  long channel_number;

  (void)ignored_initial_argument;
  keep_running = 1;

  status = initialize_cpak_subscriber_thread(context);
  if(status != 0)
    {
      exit(0);
    }

  awaited_signals = context->requestPortSignal |
                    context->cueSignal |
                    context->freeQueueSignal;

  while(keep_running != 0)
    {
      received_signals = WaitSignal(awaited_signals);

      if((received_signals & context->freeQueueSignal) != 0)
        {
          reply_freed_cpak_chunks(context);
        }

      if((received_signals & context->requestPortSignal) != 0)
        {
          while(PollForMsg(context->requestPort, 0, 0,
                           (void **)&message, &status) != 0)
            {
              switch(message->whatToDo)
                {
                case kStreamOpData:
                  status = queue_cpak_data_message(context, message);
                  break;
                case kStreamOpGetChan:
                  status = get_cpak_channel_status(context, message);
                  break;
                case kStreamOpSetChan:
                  status =
                    set_cpak_channel_subscription(context, message);
                  break;
                case kStreamOpControl:
                  status = handle_cpak_control_message(context, message);
                  break;
                case kStreamOpSync:
                  status = synchronize_cpak_subscription(context, message);
                  break;
                case kStreamOpStop:
                  status = stop_cpak_subscription(context, message);
                  break;
                case kStreamOpOpening:
                  status =
                    acknowledge_cpak_stream_opening(context, message);
                  break;
                case kStreamOpClosing:
                  status =
                    acknowledge_cpak_stream_closing(context, message);
                  keep_running = 0;
                  break;
                case kStreamOpEOF:
                  status = acknowledge_cpak_end_of_file(context, message);
                  break;
                case kStreamOpAbort:
                  status = abort_cpak_subscription(context, message);
                  keep_running = 0;
                  break;
                }

              if(message->whatToDo != kStreamOpData)
                {
                  ReplyMsg(message->msgItem, status, message, 0);
                }
            }
        }
    }

  for(channel_number = 0;
      channel_number < CPAK_MAX_CHANNELS;
      channel_number++)
    {
      flush_cpak_channel(context, channel_number);
    }
  CloseAudioFolio();
  exit(0);
}

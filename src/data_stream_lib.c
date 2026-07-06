#include "audio.h"
#include "item.h"
#include "msgport.h"

#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "kernel.h"

int32
road_rash_ds_is_marker(DSStreamCBPtr stream,
                       uint32        marker_value,
                       boolean      *is_marker)
{
  if((stream->streamFlags & STRM_GO_INPROGRESS) != 0 &&
     marker_value == stream->branchDest)
    {
      stream->streamFlags &= ~STRM_GO_INPROGRESS;
      *is_marker = true;
    }
  else
    {
      *is_marker = false;
    }
  return ROADRASH_DS_NO_ERROR;
}


boolean
stream_has_filled_buffer(DSStreamCBPtr stream)
{
  return stream->filledBufHead != 0;
}


int32
get_prerolled_data_stream_buffer_count(DSStreamCBPtr stream)
{
  
  DSDataBufPtr buffer;
  int32 buffer_count;

  
  if((stream->streamFlags & STRM_RUNNING) != 0)
    {
      return ROADRASH_DS_WAS_RUNNING_ERROR;
    }
  if((stream->streamFlags & ROADRASH_STREAM_ABORTED_FLAG) != 0)
    {
      return ROADRASH_DS_ABORT_ERROR;
    }
  if((stream->streamFlags & STRM_EOF) != 0)
    {
      return ROADRASH_DS_END_OF_FILE_ERROR;
    }

  buffer = stream->filledBufHead;
  if(buffer == 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }

  buffer_count = 1;
  while(buffer != stream->filledBufTail)
    {
      buffer_count++;
      buffer = buffer->next;
    }
  return buffer_count;
}
static Item
data_stream_port_owner(Item port_item)
{
  MsgPort *port;

  port = (MsgPort *)CheckItem(port_item, KERNELNODE, MSGPORTNODE);
  return port != 0 ? port->mp.n_Owner : 0;
}

static void
join_data_stream_request_consumers(Item *workers, long worker_count)
{
  long index;

  for(index = 0; index < worker_count; index++)
    {
      if(workers[index] > 0 && workers[index] != CURRENTTASK->t.n_Item)
        {
          dispose_data_stream_thread(workers[index]);
          while(CheckItem(workers[index], KERNELNODE, TASKNODE) != 0)
            {
              /* A failed delete is not permission to release borrowed data. */
              WaitSignal(SIGF_DEADTASK);
            }
        }
    }
}



int32
send_legacy_data_stream_request(Item            message_item,
                                boolean         asynchronous,
                                DSStreamCBPtr   stream,
                                DSRequestMsgPtr request)
{
  
  Message *message;
  MsgPort *reply_port;
  Item reply_port_item;
  Item workers[DS_MAX_SUBSCRIBERS + 3];
  long worker_count;
  long index;
  uint32 reply_signal;
  int32 status;
  int32 wait_error;

  if(request->whatToDo < kDSOpPreRollStream ||
     request->whatToDo > kDSOpConnect)
    {
      return ROADRASH_DS_INVALID_REQUEST_ERROR;
    }

  request->msgItem = message_item;
  message = (Message *)CheckItem(message_item, KERNELNODE, MESSAGENODE);
  if(message == 0)
    {
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }
  reply_port_item = message->msg_ReplyPort;
  reply_port = (MsgPort *)CheckItem(
    reply_port_item, KERNELNODE, MSGPORTNODE);
  if(reply_port == 0 ||
     (!asynchronous && reply_port->mp.n_Owner != CURRENTTASK->t.n_Item))
    {
      return ROADRASH_DS_NO_REPLY_PORT_ERROR;
    }
  reply_signal = reply_port->mp_Signal;

  
  if(asynchronous)
    {
      return SendMsg(stream->requestPort, message_item, request,
                     sizeof(DSRequestMsg));
    }


  /* Retain task Items before sending: ports can disappear during a wait. */
  worker_count = 0;
  workers[worker_count++] = stream->threadItem;
  workers[worker_count++] = data_stream_port_owner(stream->acquirePort);
  for(index = 0; index < stream->numSubscribers; index++)
    {
      workers[worker_count++] =
        data_stream_port_owner(stream->subscriber[index].subscriberPort);
    }
  if(request->whatToDo == kDSOpSubscribe)
    {
      workers[worker_count++] =
        data_stream_port_owner(request->msg.subscribe.subscriberPort);
    }
  else if(request->whatToDo == kDSOpConnect)
    {
      workers[worker_count++] =
        data_stream_port_owner(request->msg.connect.acquirePort);
    }

  if(CheckItem(workers[0], KERNELNODE, TASKNODE) == 0)
    {
      join_data_stream_request_consumers(workers, worker_count);
      stream->threadItem = 0;
      return ROADRASH_DS_ABORT_ERROR;
    }
  status = SendMsg(stream->requestPort, message_item, request,
                   sizeof(DSRequestMsg));
  if(status < 0)
    {
      return status;
    }

  wait_error = ROADRASH_DS_NO_ERROR;
  for(;;)
    {
      message = (Message *)CheckItem(message_item, KERNELNODE, MESSAGENODE);
      if(message != 0 &&
         (message->msg.n_Flags & MESSAGE_REPLIED) != 0 &&
         message->msg_MsgPort == reply_port_item)
        {
          status = GetThisMsg(message_item);
          if(status > 0)
            {
              status = (int32)message->msg_Result;
              if(status < 0 && CheckItem(workers[0], KERNELNODE, TASKNODE) == 0)
                {
                  join_data_stream_request_consumers(workers, worker_count);
                  stream->threadItem = 0;
                }
              return wait_error < 0 ? wait_error : status;
            }
          if(status < 0 && wait_error >= 0)
            {
              wait_error = status;
            }
        }

      if(message == 0 ||
         CheckItem(workers[0], KERNELNODE, TASKNODE) == 0 ||
         CheckItem(reply_port_item, KERNELNODE, MSGPORTNODE) == 0)
        {
          join_data_stream_request_consumers(workers, worker_count);
          stream->threadItem = 0;
          if(CheckItem(message_item, KERNELNODE, MESSAGENODE) != 0)
            {
              GetThisMsg(message_item);
            }
          return wait_error < 0 ? wait_error : ROADRASH_DS_ABORT_ERROR;
        }

      status = WaitSignal(reply_signal | SIGF_DEADTASK);
      if(status < 0 && wait_error >= 0)
        {
          wait_error = status;
        }
      else if((status & SIGF_ABORT) != 0 && wait_error >= 0)
        {
          wait_error = ROADRASH_DS_SIGNAL_ERROR;
        }
    }
}


int32
broadcast_legacy_stream_subscribers(DSStreamCBPtr    stream,
                                    MemPoolPtr       message_pool,
                                    SubscriberMsgPtr template_message)
{
  
  SubscriberMsgPtr message;
  DSSubscriber *subscriber;
  int32 status;
  long subscriber_index;

  
  if(message_pool->numFreeInPool < stream->numSubscribers)
    {
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }

  status = ROADRASH_DS_NO_ERROR;
  subscriber_index = 0;
  while(subscriber_index < stream->numSubscribers)
    {
      subscriber = &stream->subscriber[subscriber_index];
      message = (SubscriberMsgPtr)AllocPoolMem(message_pool);
      if(message == 0)
        {
          return ROADRASH_DS_NO_MESSAGE_ERROR;
        }

      template_message->msgItem = message->msgItem;
      *message = *template_message;
      status = SendMsg(subscriber->subscriberPort, message->msgItem,
                       message, sizeof(SubscriberMsg));
      if(status < 0)
        {
          ReturnPoolMem(message_pool, message);
          return status;
        }
      subscriber_index++;
    }
  return status;
}


int32
subscribe_legacy_data_stream(Item            message_item,
                             DSRequestMsgPtr request,
                             DSStreamCBPtr   stream,
                             DSDataType      stream_type,
                             Item            subscriber_port)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpSubscribe;
  request->msg.subscribe.dataType = stream_type;
  request->msg.subscribe.subscriberPort = subscriber_port;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
preroll_legacy_data_stream(Item            message_item,
                           DSRequestMsgPtr request,
                           DSStreamCBPtr   stream)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpPreRollStream;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
start_legacy_data_stream(Item            message_item,
                         DSRequestMsgPtr request,
                         DSStreamCBPtr   stream,
                         unsigned long   options)
{
  DSRequestMsg stack_request;
  boolean asynchronous;


  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpStartStream;
  request->msg.start.options = options;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
stop_legacy_data_stream(Item            message_item,
                        DSRequestMsgPtr request,
                        DSStreamCBPtr   stream,
                        unsigned long   options)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpStopStream;
  request->msg.stop.options = options;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
synchronize_legacy_data_stream_clock(DSStreamCBPtr stream,
                                     MemPoolPtr    message_pool,
                                     unsigned long current_time)
{
  SubscriberMsg message;
  int32 status;

  status = DSSetClock(stream, current_time);
  if(status != ROADRASH_DS_NO_ERROR)
    {
      return status;
    }

  message.whatToDo = kStreamOpSync;
  message.privatePtr = 0;
  message.msg.sync.clock = current_time;
  return broadcast_legacy_stream_subscribers(stream, message_pool,
                                             &message);
}


int32
seek_legacy_data_stream_marker(Item            message_item,
                               DSRequestMsgPtr request,
                               DSStreamCBPtr   stream,
                               unsigned long   marker_value,
                               unsigned long   options)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpGoMarker;
  request->msg.goMarker.markerValue = marker_value;
  request->msg.goMarker.options = options;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
get_legacy_data_stream_channel(Item            message_item,
                               DSRequestMsgPtr request,
                               DSStreamCBPtr   stream,
                               DSDataType      stream_type,
                               long            channel_number,
                               long           *channel_status)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpGetChannel;
  request->msg.getChannel.streamType = stream_type;
  request->msg.getChannel.channelNumber = channel_number;
  request->msg.getChannel.channelStatusPtr = channel_status;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
set_legacy_data_stream_channel(Item            message_item,
                               DSRequestMsgPtr request,
                               DSStreamCBPtr   stream,
                               DSDataType      stream_type,
                               long            channel_number,
                               long            channel_status)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpSetChannel;
  request->msg.setChannel.streamType = stream_type;
  request->msg.setChannel.channelNumber = channel_number;
  request->msg.setChannel.channelStatus = channel_status;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
control_legacy_data_stream(Item            message_item,
                           DSRequestMsgPtr request,
                           DSStreamCBPtr   stream,
                           DSDataType      stream_type,
                           long            operation,
                           void           *control_block)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpControl;
  request->msg.control.streamType = stream_type;
  request->msg.control.userDefinedOpcode = operation;
  request->msg.control.userDefinedArgPtr = control_block;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
connect_legacy_data_stream(Item            message_item,
                           DSRequestMsgPtr request,
                           DSStreamCBPtr   stream,
                           Item            acquisition_port)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpConnect;
  request->msg.connect.acquirePort = acquisition_port;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}


int32
wait_for_legacy_data_stream_end(Item            message_item,
                                DSRequestMsgPtr request,
                                DSStreamCBPtr   stream)
{
  DSRequestMsg stack_request;
  boolean asynchronous;

  asynchronous = request != 0;
  if(request == 0)
    {
      request = &stack_request;
    }
  request->whatToDo = kDSOpWaitEndOfStream;
  return send_legacy_data_stream_request(message_item, asynchronous,
                                         stream, request);
}

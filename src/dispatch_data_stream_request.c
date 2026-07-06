#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"

int
handle_data_stream_connect_request(DSStreamCBPtr   stream,
                                   DSRequestMsgPtr request);
int
handle_data_stream_preroll_request(DSStreamCBPtr stream);
int
handle_data_stream_close_request(DSStreamCBPtr   stream,
                                 DSRequestMsgPtr request);
int
handle_data_stream_wait_end_request(DSStreamCBPtr   stream,
                                    DSRequestMsgPtr request);
int
handle_data_stream_start_request(DSStreamCBPtr   stream,
                                 DSRequestMsgPtr request);
int
handle_data_stream_stop_request(DSStreamCBPtr   stream,
                                DSRequestMsgPtr request);
int
handle_data_stream_subscribe_request(DSStreamCBPtr   stream,
                                     DSRequestMsgPtr request);
int
handle_data_stream_marker_request(DSStreamCBPtr   stream,
                                  DSRequestMsgPtr request);
int
handle_data_stream_get_channel_request(DSStreamCBPtr   stream,
                                       DSRequestMsgPtr request);
int
handle_data_stream_set_channel_request(DSStreamCBPtr   stream,
                                       DSRequestMsgPtr request);
int
handle_data_stream_control_request(DSStreamCBPtr   stream,
                                   DSRequestMsgPtr request);

int32
dispatch_data_stream_request(DSStreamCBPtr   stream,
                             DSRequestMsgPtr request,
                             boolean        *keep_running,
                             int32          *request_result)
{
  int32 opcode;
  int32 result;
  DSRequestMsgPtr completed_request;

  opcode = request->whatToDo;
  if(opcode < kDSOpPreRollStream || opcode > kDSOpConnect)
    {
      ReplyMsg(request->msgItem, ROADRASH_DS_INVALID_REQUEST_ERROR,
               request, sizeof(DSRequestMsg));
      return ROADRASH_DS_INVALID_REQUEST_ERROR;
    }

  if(!*keep_running)
    {
      return ReplyMsg(request->msgItem, ROADRASH_DS_ABORT_ERROR,
                      request, sizeof(DSRequestMsg));
    }
  if(request != stream->requestMsgHead)
    {
      queue_data_stream_request(stream, request);
    }
  if(stream->repliesPending > 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }

  stream->repliesPending = 0;
  request = stream->requestMsgHead;
  switch(request->whatToDo)
    {
    case kDSOpPreRollStream:
      result = handle_data_stream_preroll_request(stream);
      break;
    case kDSOpCloseStream:
      result = handle_data_stream_close_request(stream, request);
      *keep_running = FALSE;
      break;
    case kDSOpWaitEndOfStream:
      result = handle_data_stream_wait_end_request(stream, request);
      break;
    case kDSOpStartStream:
      result = handle_data_stream_start_request(stream, request);
      break;
    case kDSOpStopStream:
      result = handle_data_stream_stop_request(stream, request);
      break;
    case kDSOpSubscribe:
      result = handle_data_stream_subscribe_request(
        stream, request);
      break;
    case kDSOpGoMarker:
      result = handle_data_stream_marker_request(stream, request);
      break;
    case kDSOpGetChannel:
      result = handle_data_stream_get_channel_request(
        stream, request);
      break;
    case kDSOpSetChannel:
      result = handle_data_stream_set_channel_request(
        stream, request);
      break;
    case kDSOpControl:
      result = handle_data_stream_control_request(stream, request);
      break;
    case kDSOpConnect:
      result = handle_data_stream_connect_request(stream, request);
      break;
    default:
      result = ROADRASH_DS_INVALID_REQUEST_ERROR;
      break;
    }
  *request_result = result;

  if(stream->repliesPending == 0 &&
     request->whatToDo != kDSOpCloseStream)
    {
      completed_request = take_next_data_stream_request(stream);
      if(completed_request->whatToDo != kDSOpWaitEndOfStream)
        {
          result = ReplyMsg(
            completed_request->msgItem, result,
            completed_request, sizeof(DSRequestMsg));
        }
    }

  return result;
}

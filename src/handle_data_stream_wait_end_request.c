#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"

int32
handle_data_stream_wait_end_request(DSStreamCBPtr   stream,
                                    DSRequestMsgPtr request)
{
  DSRequestMsgPtr previous_request;

  previous_request = stream->endOfStreamMsg;
  if(previous_request != 0)
    {
      ReplyMsg(
        previous_request->msgItem,
        ROADRASH_DS_WAIT_END_REPLACED_ERROR,
        previous_request, sizeof(DSRequestMsg));
    }
  stream->endOfStreamMsg = request;
  return ROADRASH_DS_NO_ERROR;
}

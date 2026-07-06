#ifndef ROADRASH_STREAM_RUNTIME_H
#define ROADRASH_STREAM_RUNTIME_H

#include "datastreamlib.h"

/* Road Rash embeds the Portfolio 1.2 DataStreamer implementation.  Its
 * errors predate the encoded MakeDSErr values used by later SDK releases. */
typedef enum RoadRashDataStreamError
{
  ROADRASH_DS_NO_ERROR = 0,
  ROADRASH_DS_ABORT_ERROR = -1000,
  ROADRASH_DS_END_OF_FILE_ERROR = -999,
  ROADRASH_DS_WAS_FLUSHED_ERROR = -998,
  ROADRASH_DS_NOT_RUNNING_ERROR = -997,
  ROADRASH_DS_WAS_RUNNING_ERROR = -996,
  ROADRASH_DS_NO_PORT_ERROR = -995,
  ROADRASH_DS_NO_MESSAGE_ERROR = -994,
  ROADRASH_DS_NO_MEMORY_ERROR = -992,
  ROADRASH_DS_NO_SIGNAL_ERROR = -991,
  ROADRASH_DS_SIGNAL_ERROR = -990,
  ROADRASH_DS_NO_REPLY_PORT_ERROR = -989,
  ROADRASH_DS_DUPLICATE_SUBSCRIBER_ERROR = -987,
  ROADRASH_DS_TOO_MANY_SUBSCRIBERS_ERROR = -986,
  ROADRASH_DS_BAD_BUFFER_ALIGNMENT_ERROR = -983,
  ROADRASH_DS_INVALID_CHUNK_ERROR = -982,
  ROADRASH_DS_INITIALIZATION_ERROR = -981,
  ROADRASH_DS_CLOCK_NOT_VALID_ERROR = -980,
  ROADRASH_DS_INVALID_REQUEST_ERROR = -979,
  ROADRASH_DS_NO_SUBSCRIBER_ERROR = -985,
  ROADRASH_DS_WAIT_END_REPLACED_ERROR = -978
} RoadRashDataStreamError;

#define ROADRASH_STREAM_ABORTED_FLAG 0x20U
#define ROADRASH_DS_START_RESET_CLOCK_OPTION 0x2U
#define ROADRASH_STREAM_ACTIVE_STATE_FLAGS \
        (STRM_RUNNING | STRM_CLOCK_VALID | STRM_GO_INPROGRESS)

boolean
stream_has_filled_buffer(DSStreamCBPtr stream);
int32
get_prerolled_data_stream_buffer_count(DSStreamCBPtr stream);
int32
send_legacy_data_stream_request(Item            message_item,
                                boolean         asynchronous,
                                DSStreamCBPtr   stream,
                                DSRequestMsgPtr request);
int32
road_rash_ds_is_marker(DSStreamCBPtr stream,
                       uint32        marker_value,
                       boolean      *is_marker);
int32
broadcast_legacy_stream_subscribers(DSStreamCBPtr    stream,
                                    MemPoolPtr       message_pool,
                                    SubscriberMsgPtr template_message);
int32
subscribe_legacy_data_stream(Item            message_item,
                             DSRequestMsgPtr request,
                             DSStreamCBPtr   stream,
                             DSDataType      stream_type,
                             Item            subscriber_port);
int32
preroll_legacy_data_stream(Item            message_item,
                           DSRequestMsgPtr request,
                           DSStreamCBPtr   stream);
int32
start_legacy_data_stream(Item            message_item,
                         DSRequestMsgPtr request,
                         DSStreamCBPtr   stream,
                         unsigned long   options);
int32
stop_legacy_data_stream(Item            message_item,
                        DSRequestMsgPtr request,
                        DSStreamCBPtr   stream,
                        unsigned long   options);
int32
synchronize_legacy_data_stream_clock(DSStreamCBPtr stream,
                                     MemPoolPtr    message_pool,
                                     unsigned long current_time);
int32
seek_legacy_data_stream_marker(Item            message_item,
                               DSRequestMsgPtr request,
                               DSStreamCBPtr   stream,
                               unsigned long   marker_value,
                               unsigned long   options);
int32
get_legacy_data_stream_channel(Item            message_item,
                               DSRequestMsgPtr request,
                               DSStreamCBPtr   stream,
                               DSDataType      stream_type,
                               long            channel_number,
                               long           *channel_status);
int32
set_legacy_data_stream_channel(Item            message_item,
                               DSRequestMsgPtr request,
                               DSStreamCBPtr   stream,
                               DSDataType      stream_type,
                               long            channel_number,
                               long            channel_status);
int32
control_legacy_data_stream(Item            message_item,
                           DSRequestMsgPtr request,
                           DSStreamCBPtr   stream,
                           DSDataType      stream_type,
                           long            operation,
                           void           *control_block);
int32
connect_legacy_data_stream(Item            message_item,
                           DSRequestMsgPtr request,
                           DSStreamCBPtr   stream,
                           Item            acquisition_port);
int32
wait_for_legacy_data_stream_end(Item            message_item,
                                DSRequestMsgPtr request,
                                DSStreamCBPtr   stream);

#endif

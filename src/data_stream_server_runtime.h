#ifndef ROADRASH_DATA_STREAM_SERVER_RUNTIME_H
#define ROADRASH_DATA_STREAM_SERVER_RUNTIME_H

#include "datastream.h"

int
create_data_stream_thread(void  *thread_entry,
                          int    stack_size,
                          int    priority,
                          char  *name,
                          void **stack_block,
                          int    initial_r0,
                          void  *initial_r1);
int
dispose_data_stream_thread_stack(void *stack_block);
int
dispose_data_stream_thread(Item thread_item);
int
wait_for_data_stream_thread_startup(Item thread_item, uint32 startup_signal);

void
tear_down_data_stream_context(DSStreamCBPtr stream);
DSDataBufPtr
take_free_data_stream_buffer(DSStreamCBPtr stream);
int32
request_data_stream_buffer_fill(DSStreamCBPtr stream,
                                DSDataBufPtr  buffer);
DSRequestMsgPtr
take_next_data_stream_request(DSStreamCBPtr stream);
void
queue_data_stream_request(DSStreamCBPtr   stream,
                          DSRequestMsgPtr request);
int32
release_subscriber_data_stream_buffer(DSStreamCBPtr stream,
                                      int32         subscriber_status,
                                      DSDataBufPtr  buffer);
int32
return_data_stream_buffer(DSStreamCBPtr stream,
                          DSDataBufPtr  buffer);
DSDataBufPtr
take_filled_data_stream_buffer(DSStreamCBPtr stream);
int32
find_data_stream_subscriber(DSStreamCBPtr    stream,
                            DSDataType       stream_type,
                            DSSubscriberPtr *subscriber);
int32
broadcast_data_stream_subscriber_message(DSStreamCBPtr    stream,
                                         SubscriberMsgPtr template_message);
int32
abort_data_stream(DSStreamCBPtr stream);
void
remove_data_stream_subscriber(DSStreamCBPtr   stream,
                              DSSubscriberPtr subscriber);
void
handle_data_stream_subscriber_reply(DSStreamCBPtr    stream,
                                    SubscriberMsgPtr reply,
                                    Message         *message,
                                    int32           *request_result);
void
handle_data_stream_acquisition_reply(DSStreamCBPtr stream,
                                     DataAcqMsgPtr reply,
                                     Message      *message,
                                     int32        *request_result);
int32
deliver_data_stream_buffers(DSStreamCBPtr stream);
int32
dispatch_data_stream_request(DSStreamCBPtr   stream,
                             DSRequestMsgPtr request,
                             boolean        *keep_running,
                             int32          *request_result);
int32
initialize_data_stream_thread(DSStreamCBPtr stream);
void
stream_server_thread_main(int32         ignored_initial_argument,
                          DSStreamCBPtr stream);

#endif

#ifndef CPAK_SUBSCRIBER_RUNTIME_H
#define CPAK_SUBSCRIBER_RUNTIME_H

#include "cpaksubscriber.h"

#define CPAK_QUEUE_SEMAPHORE_PRIORITY 100
#define CPAK_SEMAPHORE_NAME_BYTES 8
#define CPAK_CHANNEL_NUMBER_BYTES 2
#define CPAK_SEMAPHORE_PREFIX_TERMINATOR_INDEX 2

SubscriberMsgPtr
dequeue_cpak_channel_message(CPakContextPtr context,
                             CPakRecPtr     record);
int32
queue_cpak_chunk_for_reply(CPakContextPtr   context,
                           SubscriberMsgPtr message);
int32
poll_cpak_chunk(CPakContextPtr    context,
                CPakRecPtr        record,
                SubsChunkDataPtr *chunk_data,
                SubscriberMsgPtr *message);
int32
initialize_cpak_cel_record(DSStreamCBPtr  stream,
                           CPakContextPtr context,
                           CPakRecPtr    *record,
                           int32          channel_number,
                           int32          flush_on_sync);
int32
flush_cpak_cel(CPakContextPtr context,
               CPakRecPtr     record,
               int32          channel_number);
int32
destroy_cpak_cel_record(CPakContextPtr context,
                        CPakRecPtr     record,
                        int32          channel_number);
int32
get_cpak_current_time(CPakRecPtr record);
int32
is_time_for_next_cpak_frame(CPakRecPtr record);
int32
is_cpak_frame_ready_for_display(CPakRecPtr record);
int32
signal_cpak_freed_chunks(CPakContextPtr context);
int32
draw_cpak_frame_to_buffer(CPakContextPtr context,
                          CPakRecPtr     record,
                          Bitmap        *bitmap,
                          int32          horizontal_offset,
                          int32          vertical_offset);
void
calculate_cpak_destination(long   *base_address,
                           long   *row_bytes,
                           long    horizontal_offset,
                           long    vertical_offset,
                           long    width,
                           long    height,
                           Bitmap *bitmap);
int32
initialize_cpak_subscriber_pools(void);
int32
close_cpak_subscriber_pools(void);
int32
create_cpak_subscriber(CPakContextPtr *context,
                       int32           channel_count,
                       int32           priority_delta);
int32
dispose_cpak_subscriber(CPakContextPtr context);

int32
reply_freed_cpak_chunks(CPakContextPtr context);
int32
initialize_cpak_subscriber_thread(CPakContextPtr context);
void
cpak_subscriber_thread_main(int32          ignored_initial_argument,
                            CPakContextPtr context);

#endif

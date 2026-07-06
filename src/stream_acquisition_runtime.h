#ifndef ROADRASH_STREAM_ACQUISITION_RUNTIME_H
#define ROADRASH_STREAM_ACQUISITION_RUNTIME_H

#include "block_file_api.h"
#include "datastream.h"
#include "itempool.h"
#include "mempool.h"

enum RoadRashDataAcquisitionValue
{
  ROADRASH_STREAM_ACQUISITION_CONTEXT_BYTES = 0x78,
  ROADRASH_STREAM_ACQUISITION_IO_REQUEST_COUNT = 8
};

/*
 * Road Rash uses the Portfolio 1.2 acquisition context prefix, compiled
 * without time-based branching, plus two fields that validate completed
 * block offsets.  The public SDK AcqContext includes later branching fields
 * at 0x70 and therefore cannot describe this exact executable layout.
 */
typedef struct RoadRashDataAcquisitionContext
{
  Item creator_task;
  uint32 creator_signal;
  int32 creator_status;
  char *file_name;
  ItemPoolPtr io_request_pool;
  Item thread_item;
  void *thread_stack;
  Item request_port;
  uint32 request_port_signal;
  Item io_done_reply_port;
  uint32 io_done_reply_port_signal;
  DataAcqMsgPtr request_queue;
  DataAcqMsgPtr abort_queue;
  DataAcqMsgPtr pending_queue_head;
  DataAcqMsgPtr pending_queue_tail;
  boolean end_of_file_sent;
  int32 file_offset;
  BlockFile block_file;
  int32 expected_completed_offset;
  unsigned char completed_offset_error_seen;
  unsigned char completed_offset_alignment[3];
} RoadRashDataAcquisitionContext;

typedef RoadRashDataAcquisitionContext *RoadRashDataAcquisitionContextPtr;

typedef char RoadRashDataAcquisitionContextSizeCheck[
    sizeof(RoadRashDataAcquisitionContext) ==
    ROADRASH_STREAM_ACQUISITION_CONTEXT_BYTES
        ? 1 : -1];
typedef char RoadRashDataAcquisitionBlockFileOffsetCheck[
    offsetof(RoadRashDataAcquisitionContext, block_file) == 0x44 ? 1 : -1];
typedef char RoadRashDataAcquisitionExpectedOffsetCheck[
    offsetof(RoadRashDataAcquisitionContext, expected_completed_offset) ==
    0x70
        ? 1 : -1];
typedef char RoadRashDataAcquisitionErrorSeenOffsetCheck[
    offsetof(RoadRashDataAcquisitionContext, completed_offset_error_seen) ==
    0x74
        ? 1 : -1];

int32
queue_stream_acquisition_request(RoadRashDataAcquisitionContextPtr context,
                                 DataAcqMsgPtr                     message);
DataAcqMsgPtr
take_queued_stream_acquisition_request(RoadRashDataAcquisitionContextPtr context);
int32
flush_stream_acquisition_requests(RoadRashDataAcquisitionContextPtr context,
                                  DataAcqMsgPtr                     unused_message);

int32
initialize_stream_acquisition_context_pool(int32 context_count);
int32
dispose_stream_acquisition_context_pool(void);
int32
create_stream_acquisition_context(RoadRashDataAcquisitionContextPtr *created_context,
                                  char                              *file_name,
                                  int32                              priority_delta);
int32
dispose_stream_acquisition_context(RoadRashDataAcquisitionContextPtr context);

int32
connect_stream_acquisition(RoadRashDataAcquisitionContextPtr context,
                           DataAcqMsgPtr                     message);
int32
disconnect_stream_acquisition(RoadRashDataAcquisitionContextPtr context,
                              DataAcqMsgPtr                     message);
int32
map_stream_acquisition_marker(RoadRashDataAcquisitionContextPtr context,
                              uint32                            marker_value,
                              int32                             marker_options,
                              int32                            *file_offset);
int32
seek_stream_acquisition(RoadRashDataAcquisitionContextPtr context,
                        DataAcqMsgPtr                     message);
int32
request_stream_acquisition_read(RoadRashDataAcquisitionContextPtr context,
                                DataAcqMsgPtr                     message);

int32
initialize_stream_acquisition_thread(RoadRashDataAcquisitionContextPtr context);
Item
create_stream_acquisition_io_request(void *create_argument);
int32
open_stream_acquisition_file(RoadRashDataAcquisitionContextPtr context);
void
close_stream_acquisition_file(RoadRashDataAcquisitionContextPtr context);
void
stream_acquisition_thread_main(int32                             ignored_initial_argument,
                               RoadRashDataAcquisitionContextPtr context);

#endif

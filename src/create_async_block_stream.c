#include "stddef.h"

#include "string.h"

#include "filesystem.h"
#include "io.h"
#include "filefunctions.h"

#include "async_stream_runtime.h"
#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

enum
{
  ASYNC_BLOCK_STREAM_SECTOR_SHIFT = 11,
  ASYNC_BLOCK_STREAM_SECTOR_ALIGNMENT_MASK =
    FILESYSTEM_DEFAULT_BLOCKSIZE - 1
};

typedef char AsyncStreamBufferIoRequestOffsetIs0x08[
    offsetof(AsyncStreamBuffer, io_request) == 0x08 ? 1 : -1];
typedef char AsyncStreamBufferAllocationOffsetIs0x10[
    offsetof(AsyncStreamBuffer, allocation_base) == 0x10 ? 1 : -1];
typedef char AsyncStreamBufferDataOffsetIs0x14[
    offsetof(AsyncStreamBuffer, data) == 0x14 ? 1 : -1];
typedef char AsyncStreamBufferSizeIs0x18[
    sizeof(AsyncStreamBuffer) == 0x18 ? 1 : -1];
typedef char AsyncBlockStreamFileItemOffsetIs0x0C[
    offsetof(AsyncBlockStream, file_item) == 0x0C ? 1 : -1];
typedef char AsyncBlockStreamFileStatusOffsetIs0x10[
    offsetof(AsyncBlockStream, file_status) == 0x10 ? 1 : -1];
typedef char AsyncBlockStreamBufferingOffsetIs0x38[
    offsetof(AsyncBlockStream, buffering_enabled) == 0x38 ? 1 : -1];
typedef char AsyncBlockStreamBuffersOffsetIs0x40[
    offsetof(AsyncBlockStream, buffers) == 0x40 ? 1 : -1];
typedef char AsyncBlockStreamSizeIs0x44[
    sizeof(AsyncBlockStream) == 0x44 ? 1 : -1];

/* Reconstructed from the constructor in the original function at 0x00032778. */
AsyncBlockStream *
create_async_block_stream(const char *path,
                          int         buffer_byte_count,
                          int         buffer_count,
                          int         wait_for_buffer)
{
  IOInfo status_request;
  AsyncBlockStream *stream;
  AsyncStreamBuffer *buffer;
  int buffer_index;
  Item io_request_item;
  int result;

  if(path == 0 || buffer_byte_count <= 0 || buffer_count <= 0)
    {
      return 0;
    }
  if((buffer_byte_count & ASYNC_BLOCK_STREAM_SECTOR_ALIGNMENT_MASK) != 0)
    {
      return 0;
    }

  stream = allocate_async_block_stream_storage(
    buffer_byte_count, buffer_count);
  if(stream == 0)
    {
      return 0;
    }

  stream->file_item = OpenDiskFile((char *)path);
  if(stream->file_item < 0)
    {
      stream->last_error = stream->file_item;
      block_file_last_error = stream->last_error;
      dispose_async_block_stream_storage(stream);
      return 0;
    }

  stream->next_block = 0;
  stream->blocks_per_buffer =
    buffer_byte_count >> ASYNC_BLOCK_STREAM_SECTOR_SHIFT;
  stream->buffering_enabled = 1;
  stream->last_error = 0;

  for(buffer_index = 0;
      buffer_index < stream->buffer_count;
      buffer_index++)
    {
      buffer = &stream->buffers[buffer_index];
      buffer->state = ASYNC_STREAM_BUFFER_FREE;

      io_request_item = SF_CreateIOReqItem(stream->file_item, 0);
      buffer->io_request = io_request_item;
      if(io_request_item < 0)
        {
          stream->last_error = io_request_item;
          block_file_last_error = stream->last_error;
          dispose_async_block_stream(stream);
          return 0;
        }
      buffer->io_request_record = (IOReq *)LookupItem(io_request_item);
      if(buffer->io_request_record == 0)
        {
          stream->last_error = ASYNC_BLOCK_STREAM_ERROR_BAD_POINTER;
          block_file_last_error = stream->last_error;
          dispose_async_block_stream(stream);
          return 0;
        }
    }

  memset(&status_request, 0, sizeof(status_request));
  status_request.ioi_Command = CMD_STATUS;
  status_request.ioi_Flags = IO_QUICK;
  status_request.ioi_Recv.iob_Buffer = &stream->file_status;
  status_request.ioi_Recv.iob_Len = sizeof(stream->file_status);
  result = DoIO(stream->buffers[0].io_request, &status_request);
  if(result < 0)
    {
      stream->last_error = result;
      block_file_last_error = result;
      dispose_async_block_stream(stream);
      return 0;
    }

  if(queue_async_block_stream_reads(
       stream, (unsigned char)wait_for_buffer) != 0)
    {
      dispose_async_block_stream(stream);
      return 0;
    }

  return stream;
}

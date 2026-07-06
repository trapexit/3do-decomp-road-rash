#include "io.h"
#include "stdio.h"
#include "string.h"
#include "byte_file_runtime.h"
#include "platform.h"
#include "rw_semantic_data.h"

int
BF_FinishAsyncRead(ByteFileAsyncReadState *read_state)
{
  ByteFileReader *reader;
  BlockFileSectorCache *cache;
  int count;
  int result;
  int actual;

  if(read_state->requested_bytes <= read_state->completed_bytes)
    {
      return read_state->completed_bytes;
    }

  reader = read_state->reader;
  if(read_state->async_pending != 0)
    {
      result = WaitIO(reader->io_request_item);
      read_state->async_pending = 0;
      if(result < 0 || reader->io_request->io_Error < 0)
        {
          block_file_last_error =
            result < 0 ? result : reader->io_request->io_Error;
          read_state->pending_sector_count = 0;
          read_state->requested_bytes = read_state->completed_bytes;
          return read_state->completed_bytes;
        }
    }

  if(read_state->pending_sector_count > 0)
    {
      actual = reader->io_request->io_Actual;
      if(actual != (read_state->pending_sector_count <<
                    BYTE_FILE_SECTOR_SHIFT))
        {
          read_state->pending_sector_count = 0;
          read_state->completed_bytes += actual;
          read_state->requested_bytes = read_state->completed_bytes;
          reader->position += actual;
          return read_state->completed_bytes;
        }

      read_state->completed_bytes += actual;
      reader->position += actual;
      read_state->pending_sector_count = 0;
    }

  count = read_state->requested_bytes - read_state->completed_bytes;
  if(count > 0)
    {
      cache = &block_file_sector_cache;
      result = LockItem(cache->lock_item, SEM_WAIT);
      if(result < 0)
        {
          block_file_last_error = result;
          return read_state->completed_bytes;
        }
      cache->file_item = -1;
      cache->valid_bytes = 0;
      result = SF_Read(reader, cache->sector_buffer, 1,
                       read_state->next_sector);
      if(result < 1)
        {
          UnlockItem(cache->lock_item);
          return read_state->completed_bytes;
        }

      cache->valid_bytes = reader->io_request->io_Actual;
      cache->sector_number = read_state->next_sector;
      cache->file_item = reader->file_item;

      if(count > cache->valid_bytes)
        {
          count = cache->valid_bytes;
        }

      memcpy(read_state->destination_cursor, cache->sector_buffer, count);
      UnlockItem(cache->lock_item);
      read_state->completed_bytes += count;
      reader->position += count;
    }

  if(read_state->completed_bytes != read_state->requested_bytes)
    {
      printf(
        "ERROR in BF_FinishAsyncRead(!) Did not read all bytes requested: Requested: %ld, Read: %ld\n",
        read_state->requested_bytes, read_state->completed_bytes);
    }
  return read_state->completed_bytes;
}


int
byte_file_allocate_memory(int byte_count,
                          int memory_flags)
{
  return (int)AllocMem(byte_count, memory_flags);
}


void
byte_file_free_memory(int memory,
                      int byte_count)
{
  FreeMem((void *)memory, byte_count);
}

#include "item.h"
#include "stdio.h"
#include "string.h"
#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

static
int
byte_file_sector_number(int byte_position)
{
  if(byte_position < 0)
    {
      byte_position += BYTE_FILE_SECTOR_SIZE - 1;
    }
  return byte_position >> BYTE_FILE_SECTOR_SHIFT;
}


static
int
byte_file_sector_offset(int byte_position)
{
  unsigned int offset;

  if(byte_position < 0)
    {
      offset = (0u - (unsigned int)byte_position) &
               (BYTE_FILE_SECTOR_SIZE - 1);
      return -(int)offset;
    }
  return byte_position & (BYTE_FILE_SECTOR_SIZE - 1);
}


ByteFileAsyncReadState *
BF_StartAsyncRead(ByteFileReader         *reader,
                  void                   *destination,
                  int                     byte_count,
                  int                     position,
                  ByteFileAsyncReadState *read_state)
{
  BlockFileSectorCache *cache;
  int offset;
  int count;
  int result;
  int sectors;

  cache = &block_file_sector_cache;
  if(cache->sector_buffer == 0)
    {
      printf("ByteFile Read not initialized!");
      return 0;
    }
  if(byte_count < 0 || position < 0)
    {
      block_file_last_error = BYTE_FILE_ERROR_SEEK_RANGE;
      return 0;
    }

  reader->position = position;
  read_state->reader = reader;
  read_state->start_position = position;
  read_state->next_sector = byte_file_sector_number(position);
  read_state->requested_bytes = byte_count;
  read_state->pending_sector_count = 0;
  read_state->completed_bytes = 0;
  read_state->destination_cursor = (unsigned char *)destination;
  read_state->async_pending = 0;

  offset = byte_file_sector_offset(position);
  if(offset > 0)
    {
      result = LockItem(cache->lock_item, SEM_WAIT);
      if(result < 0)
        {
          block_file_last_error = result;
          return 0;
        }
      if(read_state->next_sector != cache->sector_number ||
         cache->file_item != reader->file_item)
        {
          cache->file_item = -1;
          cache->valid_bytes = 0;
          result = SF_Read(reader, cache->sector_buffer, 1,
                           read_state->next_sector);
          if(result < 1)
            {
              UnlockItem(cache->lock_item);
              return 0;
            }

          cache->valid_bytes = reader->io_request->io_Actual;
          cache->sector_number = read_state->next_sector;
          cache->file_item = reader->file_item;
        }

      read_state->next_sector++;
      count = BYTE_FILE_SECTOR_SIZE - offset;
      if(count > byte_count)
        {
          count = byte_count;
        }
      if(offset + count > cache->valid_bytes)
        {
          count = cache->valid_bytes - offset;
        }
      if(count < 0)
        {
          count = 0;
        }

      memcpy(destination,
             (const unsigned char *)cache->sector_buffer + offset,
             count);
      UnlockItem(cache->lock_item);
      read_state->destination_cursor += count;
      read_state->completed_bytes = count;
      reader->position += count;
    }

  sectors = byte_file_sector_number(
    byte_count - read_state->completed_bytes);
  if(sectors > 0)
    {
      result = SF_AsyncRead(reader, read_state->destination_cursor, sectors,
                            read_state->next_sector);
      if(result < 0)
        {
          block_file_last_error = result;
          return 0;
        }

      read_state->destination_cursor +=
        sectors << BYTE_FILE_SECTOR_SHIFT;
      read_state->next_sector += sectors;
      read_state->pending_sector_count = sectors;
      read_state->async_pending = 1;
    }

  return read_state;
}

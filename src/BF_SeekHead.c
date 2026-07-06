#include "item.h"

#include "byte_file_runtime.h"
#include "rw_semantic_data.h"





int
BF_SeekHead(ByteFileReader    *reader,
            int                offset,
            ByteFileSeekOrigin origin)
{
  int result;
  int sector_number;
  BlockFileSectorCache *state;

  result = BF_Seek(reader, offset, origin);
  if(result != 0)
    {
      return result;
    }

  sector_number = reader->position;
  if(sector_number < 0)
    {
      sector_number += BYTE_FILE_SECTOR_SIZE - 1;
    }
  sector_number >>= BYTE_FILE_SECTOR_SHIFT;

  state = &block_file_sector_cache;
  result = LockItem(state->lock_item, 1);
  if(result < 0)
    {
      return result;
    }
  if(sector_number == state->sector_number &&
     state->file_item == reader->file_item)
    {
      UnlockItem(state->lock_item);
      return 0;
    }

  state->file_item = -1;
  state->valid_bytes = 0;
  result = SF_Read(reader, state->sector_buffer, 1, sector_number);
  if(result < 1)
    {
      UnlockItem(state->lock_item);
      return result;
    }

  state->sector_number = sector_number;
  state->file_item = reader->file_item;
  state->valid_bytes = reader->io_request->io_Actual;
  UnlockItem(state->lock_item);
  return result;
}

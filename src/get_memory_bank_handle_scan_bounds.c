#include "memory_manager_runtime.h"

MemoryHandle *
get_memory_bank_handle_scan_bounds(MemoryHandleBank *bank,
                                   MemoryHandle    **first_handle)
{
  MemoryHandle *first;
  MemoryHandle *cursor;
  uint32 handle_count;
  uint32 flags;

  first = bank->handles;
  *first_handle = first;

  handle_count = ((bank->size_and_flags & MEM_BLOCK_SIZE_MASK) -
                  MEM_BANK_HEADER_SIZE) / sizeof(MemoryHandle);
  cursor = (MemoryHandle *)((char *)bank + MEM_BANK_HEADER_SIZE) +
           (handle_count - 1);

  while((uint32)first < (uint32)cursor)
    {
      flags = cursor->flags;
      if((flags & MEM_HANDLE_FLAG_BUSY) == 0 &&
         (flags & MEM_HANDLE_FLAG_VALID) != 0)
        {
          break;
        }
      cursor--;
    }

  return cursor;
}



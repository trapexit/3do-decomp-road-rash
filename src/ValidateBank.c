#include "memory_manager_runtime.h"

#define MEM_BLOCK_HANDLE 0x40000000U
#define MEM_BLOCK_USED 0x80000000U
#define MEM_BLOCK_SIZE_MASK 0x00FFFFFFU

#define MEM_ERR_BAD_HANDLE 0x1007
#define MEM_ERR_BAD_BANK 0x1006
#define MEM_ERR_BAD_FREE 0x100B
#define MEM_ERR_RANGE 0x100A

#define MEMORY_HEAP_RANGE_SCALE_SHIFT 3

typedef int (*MemoryMapCallback)(int error,
                                       int block,
                                       int context);



static
void
report_memory_error(int  *callback,
                          int   error,
                          char *block)
{
  MemoryMapCallback fn;

  if(callback != 0)
    {
      fn = (MemoryMapCallback)callback[0];
      fn(error, (int)block, callback[1]);
    }
}


int
ValidateBank(MemoryHeapDescriptor *heap,
             int                  *callback)
{
  MemoryBlockHeader *block;
  MemoryBlockHeader *free_block;
  MemoryHandle *handle;
  unsigned int header;
  int error;

  error = 0;
  block = (MemoryBlockHeader *)heap->base_address;
  free_block = heap->first_free_block;

  while(block != 0)
    {
      error = 0;
      header = block->size_and_flags;
      if((header & MEM_BLOCK_HANDLE) != 0)
        {
          handle = (MemoryHandle *)block->owner;
          if((char *)block + MEM_POINTER_HEADER_SIZE != handle->data)
            {
              error = MEM_ERR_BAD_HANDLE;
              report_memory_error(
                callback, error, (char *)block);
            }
        }
      else
        {
          if(block->owner != heap)
            {
              error = MEM_ERR_BAD_BANK;
              report_memory_error(
                callback, error, (char *)block);
            }

          if((header & MEM_BLOCK_USED) == 0)
            {
              if(block == free_block)
                {
                  free_block = free_block->next;
                }
              else
                {
                  error = MEM_ERR_BAD_FREE;
                  report_memory_error(
                    callback, error, (char *)block);
                }
            }
        }

      block = (MemoryBlockHeader *)
              ((char *)block + (header & MEM_BLOCK_SIZE_MASK));
      if((uint32)((char *)heap + heap->size) > (uint32)block)
        {
          continue;
        }
      if((uint32)((char *)heap->base_address +
                  (heap->size << MEMORY_HEAP_RANGE_SCALE_SHIFT)) >=
         (uint32)block)
        {
          return error;
        }

      error = MEM_ERR_RANGE;
      report_memory_error(callback, error, (char *)block);
    }

  return error;
}



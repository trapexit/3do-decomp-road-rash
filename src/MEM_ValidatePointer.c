#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

enum MemoryAddressValidationFlag
{
  MEM_ADDRESS_VALIDATE_POINTER = 1,
  MEM_ADDRESS_VALIDATE_HANDLE = 2,
  MEM_ADDRESS_VALIDATE_HANDLE_DATA = 4
};

#define MEM_ADDRESS_ALIGNMENT_MASK 3U

int
MEM_ValidatePointer(void *pointer)
{
  MemoryHeapDescriptor *heap;
  MemoryBlockHeader *block;
  int result;

  result = MEM_ERROR_BAD_ADDRESS;
  if(pointer == 0)
    {
      memory_system_last_error = result;
      return result;
    }

  LockItem(memory_allocator_globals.lock_item, 1);

  block = (MemoryBlockHeader *)
          ((char *)pointer - MEM_POINTER_HEADER_SIZE);
  heap = find_memory_bank_containing_address((uint32)block);
  if(block->owner == heap)
    {
      result = 0;
    }
  else
    {
      memory_system_last_error = MEM_ERROR_BAD_ADDRESS;
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return result;
}


int
MEM_IsValidAddress(void  *address,
                   uint32 memory_type,
                   uint32 validation_flags)
{
  MemoryHeapDescriptor *heap;
  MemoryHandle *handle;
  uint32 pointer_address;
  uint32 heap_end;

  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;
  if(heap == 0)
    {
      memory_system_last_error = 0;
      UnlockItem(memory_allocator_globals.lock_item);
      return 0;
    }

  if(address == 0)
    {
      return 0;
    }

  pointer_address = (uint32)address;
  while(heap != 0)
    {
      if(pointer_address >= (uint32)heap->base_address)
        {
          heap_end = (uint32)((char *)heap + heap->size);
          if(pointer_address <= heap_end)
            {
              if((pointer_address & MEM_ADDRESS_ALIGNMENT_MASK) != 0)
                {
                  return 0;
                }

              if(memory_type != 0 &&
                 (memory_type & heap->memory_type) == 0)
                {
                  return 0;
                }

              if(validation_flags != 0)
                {
                  if((validation_flags & MEM_ADDRESS_VALIDATE_POINTER) != 0 &&
                     MEM_ValidatePointer(address) == 0)
                    {
                      return 0;
                    }
                  if((validation_flags & MEM_ADDRESS_VALIDATE_HANDLE) != 0 &&
                     MEM_ValidateHandle((MemoryHandle *)address) == 0)
                    {
                      return 0;
                    }
                  if((validation_flags &
                      MEM_ADDRESS_VALIDATE_HANDLE_DATA) != 0)
                    {
                      handle = *(MemoryHandle **)
                               ((char *)address - MEM_HANDLE_BACK_POINTER_SIZE);
                      if(handle->data != address)
                        {
                          return 0;
                        }
                      if(MEM_ValidateHandle(handle) == 0)
                        {
                          return 0;
                        }
                    }
                }

              return 1;
            }
        }

      heap = heap->next;
    }

  return 0;
}


int
MEM_ValidateMemory(uint32 memory_type,
                   int   *callback)
{
  MemoryHeapDescriptor *heap;
  int result;

  result = 0;
  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;
  if(heap == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      UnlockItem(memory_allocator_globals.lock_item);
      return MEM_ERROR_NO_MEMORY_BANK;
    }

  MEM_ValidateMasterPtrs(callback);
  while(heap != 0)
    {
      if(memory_type == 0 ||
         (memory_type & heap->memory_type) != 0)
        {
          result = ValidateBank(heap, callback);
        }
      heap = heap->next;
    }

  return result;
}

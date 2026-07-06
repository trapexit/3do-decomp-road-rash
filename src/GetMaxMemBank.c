#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





MemoryHeapDescriptor *
GetMaxMemBank(uint32 memory_type)
{
  uint32 memtype_flags;
  uint32 largest_free_byte_count;
  MemoryHeapDescriptor *memory_bank;
  MemoryHeapDescriptor *best_bank;

  memtype_flags = memory_request_to_memtype_flags(memory_type);
  memory_bank = (MemoryHeapDescriptor *)
                memory_allocator_globals.heap_list_head;
  if(memory_bank == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      return 0;
    }

  largest_free_byte_count = 0;
  best_bank = 0;
  while(memory_bank != 0)
    {
      if((best_bank == 0 ||
          largest_free_byte_count < memory_bank->free_bytes) &&
         ((memory_bank->memory_type & memtype_flags) != 0 ||
          memtype_flags == 0))
        {
          largest_free_byte_count = memory_bank->free_bytes;
          best_bank = memory_bank;
        }
      memory_bank = memory_bank->next;
    }

  return best_bank;
}

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

#define MEM_MASTER_DUPLICATE_CALLBACK_CODE 0x100
#define MEM_MASTER_INVALID_RUN_LIMIT 3

typedef void (*MemoryValidationCallback)(int           code,
                                         MemoryHandle *entry,
                                         int           context);

int
MEM_ValidateMasterPtrs(int *callback)
{
  MemoryHandleBank *bank;
  MemoryHandle *end;
  MemoryHandle *outer;
  MemoryHandle *inner;
  MemoryHandle *next_outer;
  int previous_invalid_count;
  int invalid_count;

  bank = (MemoryHandleBank *)memory_allocator_globals.bank_pointer;
  end = (MemoryHandle *)
        ((char *)bank + (bank->size_and_flags & MEM_BLOCK_SIZE_MASK));
  outer = bank->handles;
  invalid_count = 0;

  while(outer < end)
    {
      inner = outer + 1;
      next_outer = inner;

      while(inner < end)
        {
          if((inner->flags & MEM_HANDLE_FLAG_VALID) != 0)
            {
              invalid_count = 0;
            }
          else
            {
              previous_invalid_count = invalid_count;
              invalid_count++;
              if(previous_invalid_count > MEM_MASTER_INVALID_RUN_LIMIT)
                {
                  end = inner;
                }
            }

          if(outer->data == inner->data)
            {
              if(callback != 0)
                {
                  ((MemoryValidationCallback)callback[0])(
                    MEM_MASTER_DUPLICATE_CALLBACK_CODE,
                    inner, callback[1]);
                }
              return MEM_ERROR_BAD_HANDLE;
            }

          inner++;
        }

      outer = next_outer;
    }

  return 0;
}

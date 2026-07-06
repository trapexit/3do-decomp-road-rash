#include "memory_manager_runtime.h"

void *
NewPointer(int    byte_count,
           uint32 memtype_flags,
           uint32 options)
{
  MemoryBlockHeader *allocation;
  void *pointer;

  if((options & MEM_POINTER_OPTION_ALLOW_FAILURE) == 0)
    {
      options |= MEM_POINTER_OPTION_COMPACT_FIRST;
    }
  options |= MEM_POINTER_OPTION_POINTER_BLOCK;

  allocation = NewBlock(byte_count, memtype_flags, options);
  if(allocation != 0)
    {
      pointer = (char *)allocation + MEM_POINTER_HEADER_SIZE;
    }
  else
    {
      pointer = 0;
    }

  return pointer;
}

#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





MemoryHandle *
NewHandle(int    byte_count,
          uint32 memtype_flags,
          uint32 options)
{
  uint32 purgeable;
  MemoryBlockHeader *block;
  MemoryHandle *handle;

  purgeable = options & MEM_HANDLE_OPTION_PURGEABLE;
  if(purgeable != 0 &&
     (options & MEM_POINTER_OPTION_ALLOW_FAILURE) == 0)
    {
      if(CompactBank(GetMaxMemBank(memtype_flags),
                     MEM_COMPACT_PURGEABLE_HANDLES) != 0)
        {
          return 0;
        }
    }

  block = NewBlock(byte_count, memtype_flags, options);
  if(block != 0)
    {
      handle = GetNewMasterPtr(block);
      if(handle != 0)
        {
          block->owner = handle;
          block->size_and_flags |= MEM_BLOCK_HANDLE_OWNER_FLAG;
          if(purgeable != 0)
            {
              handle->flags |= MEM_HANDLE_FLAG_BUSY;
            }
          return handle;
        }
      DisposePointer((char *)block + MEM_POINTER_HEADER_SIZE);
    }

  return 0;
}
